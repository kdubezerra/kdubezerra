#include "RayTracer.h"


RayTracer::RayTracer(int rank, int nprocs , int bl_size) : mBackgroundColor(1,1,1), mAmbientColor(0,0,0), mEyePoint(0,0,0), mSpecularColor(1,1,1)
{
	
	// begin settings
	mAmbientIntensity = 0.2;
	mRecursionLimit = 700;
	mAntiAliasDetail = 3;
	mWidth = 400;
	mHeight = 400;
	// end settings
	
	screenFileBuffer = new myPixel* [mWidth];
	for (int pixelRunner = 0; pixelRunner < mWidth; pixelRunner++) {
		screenFileBuffer[pixelRunner] = new myPixel[mHeight];
	}

	mRank = rank;
	mNProcs = nprocs;
	mBlkSize = bl_size;

	mWholeWorkSent = 0;

	mNTasks = mHeight / bl_size;
	if (mHeight % bl_size)
			mNTasks++;
	m_vTasks = new int [mNTasks];
	m_vImgLineBuffer = new unsigned char* [mNProcs];
	m_vRecebeu = new int [mNProcs];
	m_vRecv_status 	= new MPI_Status [mNProcs];
	m_vSend_status = new MPI_Status [mNProcs];
	m_vReq_recv 	= new MPI_Request [mNProcs];
	m_vReq_send 	= new MPI_Request [mNProcs];
	m_vProcesses	= new int [mNProcs];
	m_vSentTask 	=	new int [mNProcs];
	m_vProcessBlockCounter = new int [mNProcs];
	
	for (int id_proc = 0; id_proc < mNProcs; id_proc++) {
		m_vReq_recv[id_proc] = NULL;
		m_vReq_send[id_proc] = NULL;				
		m_vProcesses[id_proc] = PROCESS_READY;
// 		m_vReq_recv[id_proc] = new MPI_Request [bl_size];
		m_vImgLineBuffer[id_proc] = new unsigned char [3*mWidth + 2];
	}
	
	for (int id_task = 0; id_task < mNTasks; id_task++) {
		m_vTasks[id_task] = TASK_OPEN;		
	}

#ifdef _SDL_H	
	char rankStr[5];
	string windowCaption;	
	sprintf(rankStr, "%d", rank);
	
	if (!mRank) {
		windowCaption = "Parallel Ray Tracing, process: ";
		windowCaption += rankStr;
	}
#endif

	mOutputFilename = "t.ppm";
	mRecursions = 0;

	mpShapes = new ShapeList;
	mpLights = new LightList;
	
	/* Initialize the SDL library */
#ifdef _SDL_H
	if (!mRank) {
		if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
			cerr <<	"Couldn’t initialize SDL: " << SDL_GetError() << endl;
			exit(1);
		}
	}

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);

	/* Clean up on exit */
	atexit(SDL_Quit);
	
	/*
	* Initialize the display in a 640x480 8-bit palettized mode,
	* requesting a software surface
	*/
	if (!mRank) {		
		screen = SDL_SetVideoMode(400, 400, 8, SDL_SWSURFACE|SDL_ANYFORMAT);
		if ( screen == NULL ) {
			cerr << "Couldn’t set 400x400x8 video mode: " << SDL_GetError() << endl;
			exit(1);
		}	
	
		SDL_WM_SetCaption(windowCaption.c_str(), 0);

		cout << "Set 400x400 at " << (unsigned short) screen->format->BitsPerPixel << " bits-per-pixel mode" << endl;
	
		SDL_FillRect (screen, 0, SDL_MapRGB(screen->format, 255, 255, 255));
		SDL_UpdateRect(screen, 0, 0, 0, 0);	
	}
#endif
		
}

RayTracer::~RayTracer()
{
	// should step through mpShapes list and delete what the data points to
	delete mpShapes;

	// should step through mpLights list and delete what the data points to
	delete mpLights;

	for (int pixelRunner = 0; pixelRunner < mWidth; pixelRunner++) {
		delete [] screenFileBuffer[pixelRunner];
	}
	delete [] screenFileBuffer;

	for (int p_id = 0; p_id < mRank; p_id++) {
		delete [] m_vImgLineBuffer[p_id];
	}
	delete [] m_vImgLineBuffer;

}

void RayTracer::RayTrace() {
	int myTask = 0;
	
	if (!mRank) {
		while (!mWholeWorkSent) {
			checkDistributedTasks();
			myTask = nextTask();
			//cout << "master executing task: " << myTask << endl;
			RayTraceBlock(myTask * mBlkSize , mBlkSize);
		}
		
// 		cerr << mRank << ": saiu do\nwhile (!mWholeWorkSent) {\n  ...\n}" << endl;
		waitDistributedTasks();
// 		cerr << mRank << ": deu waitDistributedTasks();" << endl;
		finishRemoteProcesses();
		saveToFile(screenFileBuffer);
	}
	
	while (mRank && myTask != mNTasks) {
		MPI_Recv(&myTask , 1 , MPI_INT , 0 , MPI_ANY_TAG , MPI_COMM_WORLD , &m_vRecv_status[0]);
		if (myTask != mNTasks) {
			RayTraceBlock(myTask * mBlkSize , mBlkSize);
		}
	}	
	
}

void RayTracer::RayTraceBlock(int bl_start, int bl_size)
{
	
	Ray eye_ray(mEyePoint,Vector(0,0,1));
	Color draw_color;
	double i_inc, j_inc, anti_alias_i_inc, anti_alias_j_inc; // amount to increment the ray in each direction
	double i, j, anti_alias_i, anti_alias_j; // the i and j values of the ray
	int pixel_x, pixel_y, anti_alias_pixel_x, anti_alias_pixel_y; // the pixels being drawn
	
	double average_red_byte, average_green_byte, average_blue_byte;
	int anti_alias_count; // the number of anti aliases (used in averaging)
		
	/** Objeto Inserido { **/
	Color tmpSpecular;
	/** } **/
		
	i_inc = 2.0/(double)mWidth;
	j_inc = 2.0/(double)mHeight;
	anti_alias_i_inc = 1.0/(double)mAntiAliasDetail;
	anti_alias_j_inc = 1.0/(double)mAntiAliasDetail;
	
	int recebeu;
	MPI_Status rec_status;
	MPI_Status send_status;
	MPI_Request req_rcv;
	MPI_Request req_send = NULL;
	
	for (pixel_y = bl_start ; pixel_y < bl_start + bl_size   &&   pixel_y < mHeight ; /*j -= j_inc,*/ pixel_y++) 
	{
			//forma bonita de se calcular o j em funcao do pixel_y (nao funciona pq double sucks, impreciso)
			//j = 1.0 - (j_inc * pixel_y);
					
			//forma feia, mas que funciona, pq reproduz o loop que calculava o j no codigo original
		/** codigo inserido { **/
		j = 1.0;
		for (int rep = 0; rep < pixel_y; rep++)
			j -= j_inc;
		/** } **/
					
		pixel_x = 0;
		i = -1.0;
		for (; pixel_x < mWidth; i += i_inc, pixel_x++)
		{
			anti_alias_pixel_y = 0;
			anti_alias_j = 0.0;
			average_red_byte = 0;
			average_green_byte = 0;
			average_blue_byte = 0;
			anti_alias_count = 0;
			for (; anti_alias_pixel_y < mAntiAliasDetail; anti_alias_j += anti_alias_j_inc, anti_alias_pixel_y++)
			{
				anti_alias_pixel_x = 0;
				anti_alias_i = 0.0;
		
				for (; anti_alias_pixel_x < mAntiAliasDetail; anti_alias_i += anti_alias_i_inc, anti_alias_pixel_x++)
				{
					anti_alias_count++;
					eye_ray.Direction( Vector(i+(anti_alias_i*i_inc),j+(anti_alias_j*j_inc),1.0) );
		
					tmpSpecular.Red(0);
					tmpSpecular.Green(0);
					tmpSpecular.Blue(0);
								
					draw_color = Render(eye_ray, tmpSpecular);
		
					average_red_byte = average_red_byte + ((double)draw_color.RedByte() - average_red_byte)/(double)anti_alias_count;
					average_green_byte = average_green_byte + ((double)draw_color.GreenByte() - average_green_byte)/(double)anti_alias_count;
					average_blue_byte = average_blue_byte + ((double)draw_color.BlueByte() - average_blue_byte)/(double)anti_alias_count;
				}
			}
			
			handleCalculatedPixel(screenFileBuffer, m_vImgLineBuffer[mRank],
														pixel_x, pixel_y, 
														(unsigned char)average_red_byte, 
														(unsigned char)average_green_byte, 
														(unsigned char)average_blue_byte);
			
			/** TEM QUE IR PRO LOOP PRINCIPAL DO PROGRAMA **/
#ifdef _SDL_H
			SDL_Event event;
			SDL_PollEvent(&event);
			if ( event.type == SDL_QUIT ) {
				cout << "User aborted program execution" << endl;
				exit(1);
			}
#endif

			/** RECEBIMENTO DAS MENSAGENS **/
			if (!mRank)
			{
				checkDistributedTasks();	
			} /* FIM DA SECAO DO MESTRE */

				
		} /** FIM DO FOR MAIS INTERNO (COLUNAS) **/
			
		/** ENVIAR LINHA PARA O MESTRE, SE NAO FOR O PROPRIO MESTRE **/
		if (mRank) {
			MPI_Wait (&req_send, &send_status);
			bufferizeLine(pixel_y, screenFileBuffer, m_vImgLineBuffer[mRank]);
			MPI_Isend (m_vImgLineBuffer[mRank], 3*mWidth + 2, MPI_UNSIGNED_CHAR, 0, pixel_y, MPI_COMM_WORLD, &req_send);

			if ( (pixel_y + 1 >= bl_start + bl_size)
							 || (pixel_y + 1 >= mHeight) ) //ultima iteração
				MPI_Wait (&req_send, &send_status);									

		} /*else
		for (unsigned long i = 0; i < 309819587; i++);*/
			
	} /** FIM DO FOR MAIS EXTERNO (LINHAS) **/

}

Color RayTracer::Render(const Ray& rRay, Color& tmpSpecular, int tmpRecursions, bool vIsReflecting, const Shape* pReflectingFrom )
{

	/** 
	mRecursions PROVAVELMENTE será private, pois apenas diz o limite de recursoes que se deve fazer para calcular um pixel
	 **/
	// sessão critica: BEGIN
	//mRecursions++;
	// sessão critica: END
	// esse trecho de codigo pode sair daqui, e ir para a funcao RayTrace, para que fique numa "secao paralela"

	
	/** SESSAO SEGURA { **/
	Shape* closest_shape;
	Point intersect_point;
	Color result;
	if (vIsReflecting)
		closest_shape = QueryScene(rRay, intersect_point, vIsReflecting, pReflectingFrom);
	else
		closest_shape = QueryScene(rRay, intersect_point);

	if (closest_shape == NULL && !vIsReflecting)
	{
		tmpRecursions = 0;
		return mBackgroundColor;
	}
	if (closest_shape == NULL && vIsReflecting)
	{
		tmpRecursions = 0;
		return mAmbientColor*mAmbientIntensity;
	}
	if ( tmpRecursions > mRecursionLimit )
	{
		tmpRecursions = 0;
		return Color(0,0,0); // mAmbientColor*mAmbientIntensity;
	}
		
	result = closest_shape->ShapeColor()*Shade(closest_shape, intersect_point, tmpSpecular);
		
	Ray backwards_ray(intersect_point,rRay.Direction()*-1);
	
	
	
	if ( closest_shape->DiffuseReflectivity() > 0.0 )
		result = result + (Render( closest_shape->Reflect(intersect_point,backwards_ray), tmpSpecular, tmpRecursions + 1, true, closest_shape )*closest_shape->DiffuseReflectivity());

	return (result + tmpSpecular);
	
	/** } **/
}

double RayTracer::Shade(const Shape* pShapeToShade, const Point& rPointOnShapeToShade, Color& tmpSpecular)
{
	double intensity = mAmbientIntensity * pShapeToShade->AmbientReflectivity(); // the ambient intensity of the scene
	Ray light_ray; // the ray that goes from the intersection point to the light sources
	double dot_product;
	Shape* closest_shape; // the shape closest from the intersection point to the light source
	Point light_intersect; // the intersection point of the ray that goes from the intersection point to the light source 
	light_ray.Origin(rPointOnShapeToShade); // lightRay. org= object. intersect;
	Ray light_ray_from_light;

	LightListIterator iter = mpLights->begin();
	
	/** Código inserido { **/
	tmpSpecular.Red(0);
	tmpSpecular.Green(0);
	tmpSpecular.Blue(0);	
	/** } **/
	
	/** !!!SESSAO CRITICA!!! Escrita concorrente { **//*
	mSpecularColor.Red(0);
	mSpecularColor.Green(0);
	mSpecularColor.Blue(0);
							 *//** } **/
	/** A questão é que essa Color só é usada na própria Shade()
	 **/
	
	while ( iter != mpLights->end() ) // foreach light in LightList do
	{

		light_ray.Direction(Vector(rPointOnShapeToShade,(*iter)->LightPoint())); // lightRay. dir= light. dir
		light_ray_from_light.Origin((*iter)->LightPoint());
		light_ray_from_light.Direction(Vector((*iter)->LightPoint(),rPointOnShapeToShade));

		closest_shape = QueryScene(light_ray_from_light, light_intersect);
		if ( closest_shape == NULL || (closest_shape == pShapeToShade && light_ray.Direction().Dot(pShapeToShade->Normal(rPointOnShapeToShade, light_ray_from_light.Origin() )) >= 0.0 )  ) //if (QueryScene( lightRay)= NIL)
		{
			Vector normal_vector = pShapeToShade->Normal(rPointOnShapeToShade, Point() );
			dot_product = normal_vector.Dot(light_ray.Direction().Normalize());
			dot_product *= (*iter)->Intensity();

			if (dot_product < 0.0)
			{
				if (pShapeToShade->Type() == "Triangle")
					dot_product = dot_product*-1.0;
				else
					dot_product = 0.0;
			}
			intensity = unit_limiter( intensity + dot_product );

			if ( light_ray.Direction().Dot(pShapeToShade->Normal(rPointOnShapeToShade, light_ray_from_light.Origin() )) >= 0.0 )
			{
				double specular = Specular(pShapeToShade, rPointOnShapeToShade, *iter);
				tmpSpecular = tmpSpecular + Color(specular,specular,specular);
			}
		}

		iter++;
	}

	return intensity;
}

double RayTracer::Specular(const Shape* pShapeToShade, const Point& rPointOnShapeToShade, const Light* pLight)
{
	Ray reflected = pShapeToShade->Reflect(rPointOnShapeToShade,Ray(rPointOnShapeToShade, pLight->LightPoint()));

	Vector eye_vector(rPointOnShapeToShade, mEyePoint);
	Vector reflected_vector = reflected.Direction().Normalize();
	eye_vector.NormalizeThis();
	double dot_product = eye_vector.Dot(reflected_vector);	
	
	int n = pShapeToShade->SpecularSize();
	double specular_intensity = dot_product/(n - n*dot_product+ dot_product);
	return unit_limiter(specular_intensity*pLight->Intensity());
}

Shape* RayTracer::QueryScene(const Ray& rRay, Point& rIntersectionPoint, bool vIsReflecting, const Shape* pReflectingFrom)
{
	Shape* closest_shape = NULL;
	Point intersect_point;
	double closest_distance;
	double intersect_distance;
	bool found_intersection = false;

	ShapeListIterator iter = mpShapes->begin();
	while ( iter != mpShapes->end() )
	{
		if ( (*iter)->Intersect( rRay, intersect_point ) ) //essa funcao eh const, nao mexendo na shape (ufa)
		{
			intersect_distance =  intersect_point.Distance(rRay.Origin());
			if ( !found_intersection && (*iter) != pReflectingFrom)
			{
				found_intersection = true;
				rIntersectionPoint = intersect_point;
				closest_shape = *iter;
				closest_distance = intersect_distance;
			}
			else if ( intersect_distance < closest_distance && (*iter) != pReflectingFrom )
			{
				rIntersectionPoint = intersect_point;
				closest_shape = *iter;
				closest_distance = intersect_distance;
			}
		}
		iter++;
	}
	
	return closest_shape;
}

void RayTracer::AddShape(Shape* pShape)
{
	// should check if a shape with the same name already exists
	mpShapes->push_back(pShape);
}
void RayTracer::AddLight(Light* pLight)
{
	// should check if a shape with the same name already exists
	mpLights->push_back(pLight);
}

void RayTracer::BackgroundColor(const Color& rBackgroundColor)
{
	mBackgroundColor = rBackgroundColor;
}
void RayTracer::EyePoint(const Point& rEyePoint)
{
	mEyePoint = rEyePoint;
}
void RayTracer::AmbientColor(const Color& rAmbientColor)
{
	mAmbientColor = rAmbientColor;
}
void RayTracer::AmbientIntensity(double vAmbientIntensity)
{
	mAmbientIntensity = unit_limiter(vAmbientIntensity);
}

void RayTracer::OutputFilename(const string& rOutputFilename)
{
	mOutputFilename = rOutputFilename;
}

#ifdef _SDL_H
void RayTracer::drawOnScreen(SDL_Surface* screen, int x, int y, unsigned char R, unsigned char G, unsigned char B) {
	Uint32 color;
	color = SDL_MapRGB(screen->format, R, G, B);
	
	/* Lock the screen for direct access to the pixels */
	if ( SDL_LockSurface(screen) < 0 ) {
		cerr << "Can’t lock screen: " << SDL_GetError() << endl;
		return;
	}
	
	putpixel(screen, x, y, color);
	
	SDL_UpdateRect(screen, x, y, 1, 1);
	return;
	
	SDL_UnlockSurface(screen);
	
	/* Update just the part of the display that we’ve changed */
	

}
#endif

#ifdef _SDL_H
void RayTracer::putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	switch(bpp) {
		case 1:
			*p = pixel;
			break;
		case 2:
			*(Uint16 *)p = pixel;
			break;
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				p[0] = (pixel >> 16) & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = pixel & 0xff;
			} else {
				p[0] = pixel & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = (pixel >> 16) & 0xff;
			}
			break;
		case 4:
			*(Uint32 *)p = pixel;
			break;
	}
}
#endif


void RayTracer::handleCalculatedPixel(myPixel** screenBuffer, unsigned char* lineBuffer, 
																			int x, int y, unsigned char R, unsigned char G, unsigned char B)
{
	
	

#ifdef _SDL_H
	if (!mRank) {
		drawOnScreen ( screen, x, y, R, G, B );
	}
#endif
			
	
	
	screenBuffer[x][y].R = R;
	screenBuffer[x][y].G = G;
	screenBuffer[x][y].B = B;
	
	okLines[y] = 12345;
}


void RayTracer::handleReceivedLine(myPixel** screenBuffer, unsigned char* lineBuffer)
{
	int y = lineBuffer[3*mWidth] + lineBuffer[3*mWidth + 1];
	unsigned char R, G, B;
	
	for( int xRunner = 0 ; xRunner < mWidth ; xRunner++ ){
		R = lineBuffer[xRunner];
		G = lineBuffer[xRunner + mWidth];
		B = lineBuffer[xRunner + 2*mWidth];
		
#ifdef _SDL_H		
		if (!mRank) {
			drawOnScreen ( screen, xRunner , y, R, G, B );
		}
#endif
			
		screenBuffer[xRunner][y].R = R;
		screenBuffer[xRunner][y].G = G;
		screenBuffer[xRunner][y].B = B;
	}
	
	okLines[y] = 12345;
}


void RayTracer::bufferizeLine (int y, myPixel** screenBuffer, unsigned char* lineBuffer)
{
	for ( int x = 0 ; x < mWidth ; x++ )
	{
		lineBuffer[x] = screenBuffer[x][y].R;
		lineBuffer[x + mWidth] = screenBuffer[x][y].G;
		lineBuffer[x + 2*mWidth] = screenBuffer[x][y].B;

		if (y <= 255)
		{
			lineBuffer[3*mWidth] = 0;
			lineBuffer[3*mWidth + 1] = y;
		} else
		{
			lineBuffer[3*mWidth] = y - 255;
			lineBuffer[3*mWidth + 1] = 255;
		}
		
	}	
}


void RayTracer::saveToFile (myPixel** screenFileBuffer) {
	if (!mRank) {
		ofstream bufferFile(mOutputFilename.c_str(), ios::binary);
		
		unsigned char new_line = 0x0a;
		bufferFile << "P6 " << mWidth << " " << mHeight << " 255" << new_line;
		
		for (int j = 0; j < mHeight; j++)
			for (int i = 0; i < mWidth; i++) 
				bufferFile << screenFileBuffer[i][j].R << screenFileBuffer[i][j].G << screenFileBuffer[i][j].B;
		
		bufferFile.close();
	
		/** Limpando a tela **/
#ifdef _SDL_H
		SDL_FillRect (screen, 0, 0);
		SDL_UpdateRect(screen, 0, 0, 0, 0);
#endif
	
		/** testando se todas as linhas foram calculadas/recebidas**/
		if (mRank == 0)
			for( int bli = 0 ; bli < mHeight ; bli++ ){
			if (okLines[bli] != 12345)
				cout << "Processo " << mRank << ": ERRO - linha " << bli << " não calculada/recebida" << endl;
			}
	}
}


int RayTracer::checkDistributedTasks () {//retorna 0 se todas as tarefas foram concluidas
	static int working_count;
	static int proc;
	static int next_task;
	working_count = 0;
	
	for ( proc = 1 ; proc < mNProcs ; proc++ ) {
		
		if ( m_vProcesses[proc] == PROCESS_WORKING ) {
			
			working_count++;
			MPI_Test(&m_vReq_recv[proc] , &m_vRecebeu[proc] , &m_vRecv_status[proc]);
			if (m_vRecebeu[proc]) {
				
				m_vProcessBlockCounter[proc]--;
				handleReceivedLine(screenFileBuffer, m_vImgLineBuffer[proc]);
				
				if (m_vProcessBlockCounter[proc] == 0 /*|| m_vImgLineBuffer[proc][3*mWidth] == mHeight - 256*/) {				
					m_vProcesses[proc] = PROCESS_READY;
					m_vProcessBlockCounter[proc] = 0;
					working_count--;				
				}
				else {
					MPI_Irecv(m_vImgLineBuffer[proc] , 3*mWidth + 2, MPI_UNSIGNED_CHAR , proc , MPI_ANY_TAG , MPI_COMM_WORLD , &m_vReq_recv[proc]);
				}
				
			}
				
		}
		
		if ( m_vProcesses[proc] == PROCESS_READY && !mWholeWorkSent) {
			next_task = nextTask();
			sendTask(proc, next_task);
			m_vProcesses[proc] = PROCESS_WORKING;
			working_count++;
			MPI_Irecv(m_vImgLineBuffer[proc] , 3*mWidth + 2, MPI_UNSIGNED_CHAR , proc , MPI_ANY_TAG , MPI_COMM_WORLD , &m_vReq_recv[proc]);
			
			if (mHeight % mBlkSize && next_task == mNTasks - 1) {
				m_vProcessBlockCounter[proc] = mHeight % mBlkSize;				
			} else {
				m_vProcessBlockCounter[proc] = mBlkSize;
			}			
				
// 			cerr << "\nAtribuindo counter " << m_vProcessBlockCounter[proc] << " ao processo " << proc << endl << endl;
		}
		
	}
	
	return (!mWholeWorkSent + working_count);
		
}


void RayTracer::waitDistributedTasks () {//retorna 0 se todas as tarefas foram concluidas
	for ( int proc = 1 ; proc < mNProcs ; proc++ ) {
		if ( m_vProcesses[proc] == PROCESS_WORKING ) {
			
			MPI_Wait(&m_vReq_recv[proc] , &m_vRecv_status[proc]);						
			m_vProcessBlockCounter[proc]--;
			handleReceivedLine(screenFileBuffer, m_vImgLineBuffer[proc]);
				
			while (m_vProcessBlockCounter[proc] != 0) {				
// 				cerr << mRank << ": esperando pelo processo " << proc << " que teria um counter " << m_vProcessBlockCounter[proc] << endl;
				MPI_Recv(m_vImgLineBuffer[proc] , 3*mWidth + 2 , MPI_UNSIGNED_CHAR , proc , MPI_ANY_TAG , MPI_COMM_WORLD , &m_vRecv_status[proc]);				
				m_vProcessBlockCounter[proc]--;
// 				cerr << mRank << ": Recebido do processo " << proc << " que teria um novo counter " << m_vProcessBlockCounter[proc] << endl;
				handleReceivedLine(screenFileBuffer, m_vImgLineBuffer[proc]);
			}
								
		}

		m_vProcesses[proc] = PROCESS_READY;
	}
	
}


int RayTracer::nextTask() { //se o valor de retorno >= mNTasks, entao acabaram as tarefas abertas. eh so esperar que cheguem, dai.
	static int id_task = 0;
	static int ret = 0;
		
	if (id_task < mNTasks) {
		ret = id_task;
		id_task++;
		if (id_task == mNTasks) {
			mWholeWorkSent = 1;
		}
	} 
	else {
		ret = mNTasks;
	}
	
	return ret;
}


void RayTracer::sendTask (int p_id, int task_number) {
	m_vSentTask[p_id] = task_number;
	MPI_Isend (&m_vSentTask[p_id], 1, MPI_INT, p_id, 0, MPI_COMM_WORLD, &m_vReq_send[p_id]);
	//cout << "master -> p_" << p_id << ", task: " << task_number << endl;
}

void RayTracer::finishRemoteProcesses() {
	for (int p_id = 1 ; p_id < mNProcs ; p_id++) {
		sendTask(p_id , mNTasks);
	}
}

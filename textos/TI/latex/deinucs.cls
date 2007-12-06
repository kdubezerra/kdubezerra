%==============================================================================
% deinucs - classe de documento com padrões do DEIN/UCS
% $Id: deinucs.cls,v 1.0 2002/06/29 10:30:00 avila Exp $
%
% Copyright (C) 2002-2002 DEIN-UCS 
%
% This program is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 2 of the License, or
% (at your option) any later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with this program; if not, write to the Free Software
% Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
%
% DEIN - UCS TeX Users Group
% Department of Informatics --- UCS
% Caxias do Sul, Brazil
% http://dein.ucs.br
% 
%==============================================================================

%==============================================================================
% Identificacao
%==============================================================================
\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{deinucs}[2002/06/29 v1.0.0 Formatacao de documentos para o DEIN/UCS]

%==============================================================================
% Declaracao de opcoes
%==============================================================================
\DeclareOption{rpucs}{
    \AtEndOfClass{\input{rpucs.def}}
}

\DeclareOption{relat-projetoucs}{
    \AtEndOfClass{\input{relat-projetoucs.def}}
}

\DeclareOption{prop-tiucs}{
    \AtEndOfClass{\input{prop-tiucs.def}}
}

\DeclareOption{prop-teseucs}{
    \AtEndOfClass{\input{prop-teseucs.def}}
}

\DeclareOption{equcs}{
    \AtEndOfClass{\input{equcs.def}}
}

\DeclareOption{plano-doutoradoucs}{
    \AtEndOfClass{\input{plano-doutoradoucs.def}}
}

\DeclareOption{relat-icucs}{
    \AtEndOfClass{\input{relat-icucs.def}}
}

\DeclareOption{propdipl}{
    \AtEndOfClass{\input{propdipl.def}}
}

\DeclareOption{diplucs}{
    \AtEndOfClass{\input{diplucs.def}}
}

\DeclareOption{tiucs}{
    \AtEndOfClass{\input{tiucs.def}}
}

\DeclareOption{pepucs}{
    \AtEndOfClass{\input{pepucs.def}}
}

\DeclareOption{dissucs}{
    \AtEndOfClass{\input{dissucs.def}}
}

\DeclareOption{teseucs}{
    \AtEndOfClass{\input{teseucs.def}}
}

\DeclareOption*{
    \PassOptionsToClass{\CurrentOption}{report}
}

%==============================================================================
% Declaracoes preliminares
%==============================================================================
\ProcessOptions
\LoadClass[12pt,a4paper]{report}
\RequirePackage[english,brazilian]{babel}
\RequirePackage{deindefs}
\AtEndDocument{
    \label{totalpages}
}

%==============================================================================
% Definicao de strings
%==============================================================================
\newcommand{\coursename}{\ppgc}
\newcommand{\advisorname}{Orientador}
\newcommand{\coadvisorname}{Co-orientador}
\def\ackname{Agradecimentos}
\def\listabbrvname{Lista de Abreviaturas}
\def\anexoname{Anexo}
\def\bibliografianame{Bibliografia}
\def\keywordname{Palavras-chave}
\def\enkeywordname{Keywords}
\def\BibTeX{{\rm B\kern-.05em{\sc i\kern-.025em b}\kern-.08em}\TeX}

%==============================================================================
% medidas diversas
%==============================================================================
\newlength{\dein@topmargin}
\newlength{\dein@bottommargin}
\newlength{\dein@sidemargin}
\newlength{\dein@titlepagewinwidth}
\newlength{\dein@titlepagewinheight}
\newlength{\dein@titlepagewinskip}
\newlength{\dein@titleboxheight}
\newlength{\dein@authorboxheight}
\newlength{\dein@tmplen}
\setlength{\dein@topmargin}{30mm}
\setlength{\dein@bottommargin}{20mm}
\setlength{\dein@sidemargin}{30mm}
\setlength{\dein@titlepagewinwidth}{90mm}
\setlength{\dein@titlepagewinheight}{50mm}
\setlength{\dein@titlepagewinskip}{30mm}

%==============================================================================
% Layout do documento
%==============================================================================
% medidas na vertical
% as definicoes de \topmargin, \headheight e \headsep sao feitas de modo
% a deixar o numero da pagina centralizado verticalmente
\setlength{\dein@tmplen}{\dein@topmargin}
    \addtolength{\dein@tmplen}{-\baselineskip}
    \setlength{\dein@tmplen}{.5\dein@tmplen}
\setlength{\topmargin}{\dein@tmplen}
\setlength{\headheight}{\baselineskip}
\setlength{\headsep}{\dein@tmplen}
\setlength{\textheight}{\paperheight}
    \addtolength{\textheight}{-\dein@topmargin}
    \addtolength{\textheight}{-\dein@bottommargin}
\setlength{\footskip}{.5\dein@bottommargin}

% medidas na horizontal
\setlength{\oddsidemargin}{\dein@sidemargin}
\setlength{\evensidemargin}{\dein@sidemargin}
\setlength{\textwidth}{\paperwidth}
    \addtolength{\textwidth}{-\dein@sidemargin}
    \addtolength{\textwidth}{-\dein@sidemargin}

% ajuste das margens iniciais
\addtolength{\topmargin}{-1in}  
\addtolength{\oddsidemargin}{-1in}
\addtolength{\evensidemargin}{-1in}

% indentação dos paragrafos e etc.
\setlength{\parindent}{2.5em}
\setlength{\belowcaptionskip}{1.5ex}

%==============================================================================
% Numeracao das paginas
%==============================================================================
\def\ps@deinucs{
    \let\@oddfoot\@empty
    \let\@evenfoot\@empty
    \def\@evenhead{\centerline{\thepage}}
    \def\@oddhead{\centerline{\thepage}}
}
\pagestyle{deinucs}

%==============================================================================
% Comandos para definição de autores, orientador e co-orientador
%==============================================================================
\newcounter{dein@authorcount}
\renewcommand{\author}[2]{
    \addtocounter{dein@authorcount}{1}
    \expandafter\gdef\csname @author\alph{dein@authorcount}last\endcsname{#1}
    \expandafter\gdef\csname @author\alph{dein@authorcount}first\endcsname{#2}
    \expandafter\gdef\csname @author\alph{dein@authorcount}\endcsname{#2 #1}
}

\DeclareRobustCommand{\advisor}[3][]{
    \gdef\@advisorlast{#2}
    \gdef\@advisorfirst{#3}
    \gdef\@advisor{#1~#3 #2}
}

\DeclareRobustCommand{\coadvisor}[3][]{
    \gdef\@coadvisorlast{#2}
    \gdef\@coadvisorfirst{#3}
    \gdef\@coadvisor{#1~#3 #2}
}

%==============================================================================
% Comando para definição das palavras-chave
%==============================================================================
\newcounter{kwcounter}
\newcommand{\keyword}[1]{
    \addtocounter{kwcounter}{1}
    \expandafter\gdef\csname keyw\alph{kwcounter}\endcsname{#1}
}

\newcommand{\@cipkw}{
    \@ifundefined{keywa}{}{%
        1.~\expandafter\MakeUppercase\keywa\@.
    \@ifundefined{keywb}{}{%
        2.~\expandafter\MakeUppercase\keywb\@.
    \@ifundefined{keywc}{}{%
        3.~\expandafter\MakeUppercase\keywc\@.
    \@ifundefined{keywd}{}{%
        4.~\expandafter\MakeUppercase\keywd\@.
    \@ifundefined{keywe}{}{%
        5.~\expandafter\MakeUppercase\keywe\@.
    \@ifundefined{keywf}{}{%
        6.~\expandafter\MakeUppercase\keywf\@.
    \@ifundefined{keywg}{}{%
        7.~\expandafter\MakeUppercase\keywg\@.
    \@ifundefined{keywh}{}{%
        8.~\expandafter\MakeUppercase\keywh\@.
    \@ifundefined{keywi}{}{%
        9.~\expandafter\MakeUppercase\keywi\@.
    \@ifundefined{keywj}{}{%
        10.~\expandafter\MakeUppercase\keywj\@.
    }}}}}}}}}}
}

\newcommand{\@abstractkw}{
    \@ifundefined{keywa}{}{%
        \expandafter\MakeUppercase\keywa
    \@ifundefined{keywb}{}{%
        , \keywb
    \@ifundefined{keywc}{}{%
        , \keywc
    \@ifundefined{keywd}{}{%
        , \keywd
    \@ifundefined{keywe}{}{%
        , \keywe
    \@ifundefined{keywf}{}{%
        , \keywf
    \@ifundefined{keywg}{}{%
        , \keywg
    \@ifundefined{keywh}{}{%
        , \keywh
    \@ifundefined{keywi}{}{%
        , \keywi
    \@ifundefined{keywj}{}{%
        , \keywj
    }}}}}}}}}}%
}

%==============================================================================
% Definição da data da defesa \date{fevereiro}{1999}
%==============================================================================
\gdef\@mespublicacao{
    \ifcase\month\or
        janeiro\or
        fevereiro\or
        mar{\c{c}}o\or
        abril\or
        maio\or
        junho\or
        julho\or
        agosto\or
        setembro\or
        outubro\or
        novembro\or
        dezembro
    \fi
}

\gdef\@anopublicacao{\number\year}

\DeclareRobustCommand{\date}[2]{
    \gdef\@mespublicacao{#1}
    \gdef\@anopublicacao{#2}
}

%==============================================================================
% Definição padrao da folha de rosto
% Para mudar os dados na folha de rosto basta redefinir os comandos
% usados abaixo
%==============================================================================
\newcommand{\titlepagetopinfo}{
        \MakeUppercase{\ucs}\\
        \MakeUppercase{\dein}\\
        \MakeUppercase{\coursename}
}

\newcommand{\titlepagebottominfo}{
    \vfill
    Caxias~do~Sul, \@mespublicacao\ de~\@anopublicacao.
}

\def\dein@titlebox{
    \parbox[b]{\dein@titlepagewinwidth}{
        \centering\large\textbf{\@title}
    }
}

\def\dein@authorbox{
    \parbox[b]{\dein@titlepagewinwidth}{
        \centering\MakeUppercase{\@authora}
        \@ifundefined{@authorb}{}{\\\MakeUppercase{\@authorb}
        \@ifundefined{@authorc}{}{\\\MakeUppercase{\@authorc}
        \@ifundefined{@authord}{}{\\\MakeUppercase{\@authord}
    }}}}
}

\DeclareRobustCommand{\titlepagegeneralinfo}{
    \settoheight{\dein@titleboxheight}{\dein@titlebox}
    \settoheight{\dein@authorboxheight}{\dein@authorbox}
    \setlength{\dein@tmplen}{\dein@titlepagewinheight}
        \addtolength{\dein@tmplen}{-\dein@titleboxheight}
        \addtolength{\dein@tmplen}{-\dein@authorboxheight}
        \addtolength{\dein@tmplen}{-\baselineskip} % linha do `por'
        \setlength{\dein@tmplen}{.5\dein@tmplen}
    \\\vspace{\dein@titlepagewinskip}
%   \fbox{
    \parbox[t]{\dein@titlepagewinwidth}{
        \centering
        \dein@titlebox\\[\dein@tmplen]
        por\\[\dein@tmplen]
        \dein@authorbox
    }
%   }
}

\DeclareRobustCommand{\titlepagespecificinfo}{}

\newcommand{\titlepageadvisorinfo}{
    \@ifundefined{@advisor}{}{\vfill\@advisor\\\advisorname}
    \@ifundefined{@coadvisor}{}{\vfill\@coadvisor\\\coadvisorname}
}

\DeclareRobustCommand{\thetitlepage}{
    \begin{center}
        \titlepagetopinfo
        \titlepagegeneralinfo
        \titlepagespecificinfo
        \titlepageadvisorinfo
        \titlepagebottominfo
    \end{center}
}

\DeclareRobustCommand{\maketitle}{
    \begin{titlepage}
        \thetitlepage
    \end{titlepage}
    \setcounter{page}{2}
}

%==============================================================================
% definicao da nominata
%==============================================================================
\gdef\@nominata{
    \MakeUppercase{\ucs}\\
    Reitora: Prof\textsuperscript{a}.~Wrana Maria Panizzi\\
    Pr{\'o}-Reitor de Ensino: Prof.~Jos{\'e} Carlos Ferraz Hennemann\\
    Pr{\'o}-Reitor Adjunto de P{\'o}s-Gradua{\c{c}}{\~a}o: Prof.~Jaime Evaldo Fernsterseifer\\ 
    Diretor do Instituto de Inform{\'a}tica: Prof.~Philippe Olivier Alexandre Navaux\\
    Coordenador do PPGC: Prof.~Carlos Alberto Heuser\\
    Bibliotec{\'a}ria-chefe do Instituto de Inform{\'a}tica: Beatriz Regina Bastos Haro
}

%==============================================================================
% catalogacao na fonte (CIP)
%==============================================================================
\DeclareRobustCommand{\cip}{
    \clearpage
    \begin{center}
        \textbf{CIP --- CATALOGA\c{C}\~{A}O NA PUBLICA\c{C}\~{A}O}\\[2ex]

        % quadro
        \framebox[120mm]{
            \begin{minipage}{110mm}
                \vspace*{0.2ex}
                \setlength{\parindent}{1.5em}
                \setlength{\parskip}{1.3ex}
                \noindent\@authoralast, \@authorafirst
                \par\@title\ / por \@authora.\ --- Caxias~do~Sul: PPGC da UCS, \@anopublicacao.
                \par\pageref{totalpages} f.: il.\@cipspecificinfo
                \par\@cipkw I.~\@advisorlast, \@advisorfirst. \@ifundefined{@coadvisorfirst}{DEIN.~T\'{\i}tulo.}{DEIN.~\@coadvisorlast, \@coadvisorfirst. DEINI.~T\'{\i}tulo.}\\[0.2ex]
            \end{minipage}
        }
    \end{center}
    \vfill
    \@nominata
}

%==============================================================================
% Dedicatória
%==============================================================================
\newenvironment{dedicatoria}{
    \clearpage\vspace*{\fill}
    \begin{flushright}
}{
    \end{flushright}
}

%==============================================================================
% Agradecimentos
%==============================================================================
\newenvironment{agradecimentos}{
    \chapter*{\ackname}
}{
}

%==============================================================================
% Ajuste do sumário (nada indentado, tudo alinhado à esquerda) 
%==============================================================================
\DeclareRobustCommand{\l@chapter}[2]{
    \ifnum \c@tocdepth >\m@ne
        \addpenalty{-\@highpenalty}
        \vskip -1ex \@plus\p@
        \setlength\@tempdima{1.5em}
        \begingroup
            \parindent \z@ \rightskip \@pnumwidth
            \parfillskip -\@pnumwidth
            \leavevmode
            \advance\leftskip\@tempdima
            \hskip -\leftskip
            \@dottedtocline{0}{0em}{1.8em}{\large\bfseries#1}{#2}
            \penalty\@highpenalty
        \endgroup
    \fi
}

% section
\DeclareRobustCommand{\l@section}[2]{
    \@dottedtocline{1}{0em}{2.8em}{\textbf{#1}}{#2}
}

% subsection
\DeclareRobustCommand{\l@subsection}{
    \@dottedtocline{2}{0em}{3.7em}
}

%==============================================================================
% Definicao da lista de abreviaturas
%==============================================================================
\def\@deinucsformatlabel#1{\textbf{#1}}
\newenvironment{listofabbrv}[1]{
    \chapter*{\listabbrvname}
    \addcontentsline{toc}{chapter}{\listabbrvname}
    \begin{list}{\textbf{??}}{
        \settowidth{\labelwidth}{\@deinucsformatlabel{#1}}
        \setlength{\labelsep}{1em}
        \setlength{\itemindent}{0mm}
        \setlength{\leftmargin}{\labelwidth}
        \addtolength{\leftmargin}{\labelsep}
        \setlength{\rightmargin}{0mm}
        \setlength{\itemsep}{.1\baselineskip}
        \renewcommand{\makelabel}[1]{\parbox[b]{\labelwidth}{\@deinucsformatlabel{##1}}}
    }
}{
    \end{list}
}

%==============================================================================
% Redefinicao das listas de figuras e de tabelas (para fazer incluir no
% sumario)
%==============================================================================
\DeclareRobustCommand{\listoffigures}{
    \chapter*{\listfigurename}
    \addcontentsline{toc}{chapter}{\listfigurename}
    \@starttoc{lof}
}

\DeclareRobustCommand{\listoftables}{
    \chapter*{\listtablename}
    \addcontentsline{toc}{chapter}{\listtablename}
    \@starttoc{lot}
}

%==============================================================================
% Resumo (abstract) e Abstract (englishabstract)
%==============================================================================
\renewenvironment{abstract}{
    \chapter*{\abstractname}
    \DeclareRobustCommand{\keywordsname}{Palavras-chave}
    \addcontentsline{toc}{chapter}{\abstractname}
}{
    \par\vspace{0.5\baselineskip}
    \noindent{\bfseries\keywordname:} \@abstractkw.
}

\newenvironment{englishabstract}[2]{
    \clearpage
    \noindent\textbf{TITLE:} ``\MakeUppercase{#1}''
    \vspace{5ex}\par
    \selectlanguage{english}
    \@makeschapterhead{\abstractname}
    \DeclareRobustCommand{\keywordsname}{Keywords}
    \addcontentsline{toc}{chapter}{\abstractname}
    \gdef\@enkeywords{#2}
}{
    \par\vspace{0.5\baselineskip}
    \noindent{\bfseries\enkeywordname:} \@enkeywords.
        \selectlanguage{brazilian}
}

%==============================================================================
% Formatação dos capítulos
%==============================================================================
% apesar de não definir a formatação propriamente dita, este comando tem que
% ser redefinido para que o primeiro parágrafo seja indentado, e para que o
% estilo de numeração desta página fique igual ao das demais
\DeclareRobustCommand{\chapter}{
    \clearpage
    \global\@topnum\z@  % impede colocação de ``floats'' no topo desta página
    \secdef\@chapter\@schapter  % faz a formatação
}

% aqui se define a formatação para capítulos numerados
\def\@makechapterhead#1{
    \reset@font
    {\noindent\Large\bfseries\thechapter\hspace{1em}#1\par}
    \vspace{5ex}
}

% e aqui para os não numerados (como, por exemplo, a bibliografia)
\def\@makeschapterhead#1{
    \reset@font
    \centerline{\Large\bfseries#1}\par
    \vspace{5ex}
}

\DeclareRobustCommand{\appendix}{
    \setcounter{chapter}{0}
    \setcounter{section}{0}
    \renewcommand\@chapapp{\anexoname}
    \renewcommand\thechapter{\@Alph\c@chapter}
    \def\@makechapterhead##1{
        \reset@font
        {\noindent\Large\bfseries\anexoname~\@arabic\c@chapter\hspace{1em}##1\par}
        \vspace{5ex}
    }
    \def\@chapter[##1]##2{
        \refstepcounter{chapter}
        \addcontentsline{toc}{chapter}{\protect\anexoname~\numberline{\@arabic\c@chapter}##1}
        \chaptermark{##1}
        \addtocontents{lof}{\protect\addvspace{10\p@}}
        \addtocontents{lot}{\protect\addvspace{10\p@}}
        \@makechapterhead{##2}
        \@afterheading
    }
}

%==============================================================================
% Formatação das seções
%==============================================================================
\DeclareRobustCommand{\section}{
    \@startsection{section}{1}{0pt}{5ex}{2ex}{\reset@font\large\bfseries}
}

\DeclareRobustCommand{\subsection}{
    \@startsection{subsection}{2}{0pt}{3ex}{1.5ex}{\reset@font\bfseries}
}

% O PPGC determina que sejam usados no máximo 3 níveis de seções, incluindo
% o capítulo. Meio chato, mas...
\DeclareRobustCommand{\subsubsection}{
    \ClassWarning{deinucs}{Ignoring \subsubsection}
}

\DeclareRobustCommand{\paragraph}{
    \ClassWarning{deinucs}{Ignoring \paragraph}
}

\DeclareRobustCommand{\subparagraph}{
    \ClassWarning{deinucs}{Ignoring \subparagraph}
}

%==============================================================================
% Redefinição das captions
%==============================================================================
\def\@makecaption#1#2{
    \vspace{\abovecaptionskip}
    \parbox{\textwidth}{\centering#1~--~#2}% nao remover este `%'
    \vspace{\belowcaptionskip}
}

\newlength{\deinu@marginwidth}
\def\deinu@rigidspace{\hspace*{.4em}}
\DeclareRobustCommand{\deinu@redefmargin}{\renewcommand*\l@figure{\@dottedtocline{1}{0em}{\deinu@marginwidth}}}
\long\def\@caption#1[#2]#3{
    \addtocontents{\csname ext@#1\endcsname}{
        \settowidth{\deinu@marginwidth}{\MakeUppercase{\csname#1name\endcsname}~\csname the#1\endcsname\deinu@rigidspace--\deinu@rigidspace}\deinu@redefmargin
    }
    \addcontentsline{\csname ext@#1\endcsname}{#1}{\noindent\MakeUppercase{\csname#1name\endcsname}~\csname the#1\endcsname\deinu@rigidspace--\deinu@rigidspace{#2}}
    \par\begingroup
        \@parboxrestore
        \normalsize
        \@makecaption{\MakeUppercase{\csname fnum@#1\endcsname}}{\ignorespaces #3}\par
    \endgroup
}

%==============================================================================
% Redefinição da bibliografia
%==============================================================================
\renewenvironment{thebibliography}[1]{
    \chapter*{\bibliografianame}
    \addcontentsline{toc}{chapter}{\bibliografianame}
    \newcounter{deinucsbibcounter}
    \begin{list}{\@biblabel{\arabic{deinucsbibcounter}}}{
        \usecounter{deinucsbibcounter}
        \settowidth{\labelwidth}{\@biblabel{#1}}
        \setlength{\labelsep}{1.2em}
        \setlength{\itemindent}{0mm}
        \setlength{\leftmargin}{\labelwidth}
        \addtolength{\leftmargin}{\labelsep}
        \setlength{\rightmargin}{0mm}
        \setlength{\itemsep}{1.2ex}}
        \sloppy
        \clubpenalty4000
        \@clubpenalty\clubpenalty
        \widowpenalty4000
        \sfcode`\.\@m
}{
        \def\@noitemerr
        {\@latex@warning{Bibliografia vazia?}}
    \end{list}
}

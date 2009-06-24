Os arquivos neste diretório contém as macros LaTeX necessárias para 
você imprimir seu artigo no formato requerido pelo WSCAD 2009. 

O tamanho de folha utilizado é de 8.5 x 11 polegadas (tamanho 
carta).

Os arquivos neste diretório são os seguintes: 

README.TXT    -- Este arquivo. 
wscad2009.sty -- o arquivo de estilo
wscad2009.tex -- o conjunto principal de macros
wscad2009.bib -- exemplo de macros para a bibliografia
wscad2009.bst -- o conjunto principal de macros para bibliografia 
wscad2009.pdf -- o arquivo resultado do processamento 
		 do arquivo wscad2009.tex mostrando como deve
                 ficar a formatação final do seu artigo.

Para processar esses arquivos utilize os seguintes comandos em 
um ambiente do tipo UNIX:

latex wscad2009.tex
bibtex wscad2009.aux
latex wscad2009.tex  
latex wscad2009.tex  -- Sim é preciso executar novamente duas vezes
dvipdf wscad2009.dvi 



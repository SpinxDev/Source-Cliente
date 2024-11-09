# <span style="color: #2ecc71;">Source do Cliente de Perfect World, versão 1.5.2~1.5.3 customizada</span>

### <span style="color: #3498db;">Download Visual Studio 2005 + DXSDK + IFC + DX81SDK (Tudo necessário para compilar o cliente):</span>

    https://www.mediafire.com/folder/8vfqmbhpauopf/PW+Src+Cliente+-+Necessarios

### <span style="color: #3498db;">Download do cliente completo: </span>

    https://www.mediafire.com/file/yb4oqwqri1ab02q/Perfect+World+1.5.2-1.5.3_Customizada__Alien.rar/file

### <span style="color: #3498db;">Como instalar e configurar a compilação</span>
    1- Instalar o visual studio 2005
    
    2- Instalar dx81sdk_full.exe
    
    3- Instalar dxsdk_sum2004.exe
    
    4- Extrair o v6.A.zip
    
    5- Extrair o IFC.zip


Após instalado, abrir o visual studio e ir em: 

    "Tools > Options -> Projects and Solutions"

    - Adicionar a pasta include do SDK 6.0A, Directx e IFC na parte de "Include Files"
    - Adicionar a pasta Lib do SDK 6.0A, Directx e IFC na parte de "Library Files"

  
![imagem](https://i.imgur.com/G2Jng9o.png)
![imagem2](https://i.imgur.com/HWhjcJE.png)

Compile o projeto na seguinte ordem: (se for utilizar debug, compile so o que tiver com nome debug, e vice versa)

=========================================	

\Angelica2\Angelica2.sln

	- Debug	
	- Unicode Debug
	- Release
	- Unicode Release
	
\Common\Common.sln

	- StdNewDebug 	- build luawrapper
	- StdNewRelease - build luawrapper
	- Debug - build Gfxcommon_NoAudio
	- Release - build Gfxcommon_NoAudio
	- Editor Unicode Debug - build Gfxcommon_NoAudio
	- Editor Unicode Release - build Gfxcommon_NoAudio
	- EditorDebug - build solution
	- EditorRelease - build solution
	- UnicodeDebug - build solution
	- UnicodeRelease - build solution
	
\Element\pw.sln

    - Debug
    - Release

## <span style="color: #3498db;">Informações das alterações do cliente:</span>
### <span style="color: #3498db;">Versão 1.5.2 ~ 1.5.5 customizada</span>


Para saber das adições basta pesquisar por "alien" pelo codigo no projeto

### <span style="color: #3498db;">Adições & Modificações:</span>
    1- Mostra a lista de clãs ao apertar G(sem cla)/J(com cla) (tem que consertar algumas descrições)
    
    2- Feito grande parte somente para testes do Transmutador, está comentado /**/, basta remover
    
    3- Adicionado contador de buffs/debuffs/skills
    
    4- Adicionado acesso rápido à funções no inventário
    
    5- Feito uma pequena parte da Paleta da Fortuna
    
    6- Editado o cubo do destino, comecei a fazer estilo Praça Mágica (MIR4), mas parei por preguiça
    
    7- Adicionado icone para descrição de drops de mobs (tentei fazer para aparecer também os itens vendidos na lojinha caso fosse player, mas parei)
    
    8- Adicionado luz de identificação de missões nos mobs/materiais/minerais/ervas
    
    9- Aprimoramento do Auto Cultivo
    
    10- Adicionado skill sender (ja ativado) para desativar basta comentar as 2 linhas entre Skill Sender - Inicio e Skill Sender - Fim
    
    11- Adicionado Astrolábio (foi feita infusão, nao foi terminado o resto)
    
    12- Adicionado Pedra de Teleporte Tempestuoso (59410)



Tem mais coisa, é só pesquisar "alien" pelo codigo no projeto

<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <title>Anotaciones en Amaya</title>
  <meta name="GENERATOR" content="amaya 5.2" />
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
</head>

<body xml:lang="es" lang="es">

<table border="0" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../images/w3c_home" /> <img alt="Amaya"
        src="../images/amaya.gif" /></td>
      <td><p><a href="MakeBook.html" accesskey="p"><img alt="previa"
        src="../images/left.gif" /></a> <a href="Manual.html"
        accesskey="t"><img alt="arriba" src="../images/up.gif" /></a> <a
        href="Configure.html" accesskey="n"><img alt="siguiente"
        src="../images/right.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>
<ul>
  <li><a href="#What">�Qu� es una Anotaci�n?</a></li>
  <li><a href="#Local">Anotaciones Locales y Remotas</a></li>
  <li><a href="#Annotation">El Men� Anotaciones</a></li>
  <li><a href="#Configurat">Preparando la Configuraci�n para las
    Anotaciones</a></li>
  <li><a href="#Creating1">Creando una Anotaci�n</a></li>
  <li><a href="#Deleting">Borrando una Anotaci�n</a></li>
  <li><a href="#Loading">Cargando y Presentando Anotaciones</a></li>
  <li><a href="#Navigating">Navegando entre Anotaciones</a></li>
  <li><a href="#Moving">Moviendo Anotaciones</a></li>
  <li><a href="#Issues1">Problemas con Anotaciones y Documentos
    Modificados</a></li>
</ul>

<div id="Annotations">
<h2>Anotaciones en Amaya</h2>

<h3><a name="What">�Qu� es una Anotaci�n?</a></h3>

<p>La anotaciones son comentarios, notas, explicaciones, u otro tipo de
observaciones que pueden adjuntarse a un documento Web o a una parte
seleccionada del documento. Como son externas es posible anotar cualquier
documento Web de manera independiente, sin necesidad de editar ese documento.
Desde el punto de vista t�cnico, las anotaciones normalmente son vistas como
<b>metadatos</b>, ya que dan informaci�n adicional sobre una porci�n
existente de datos. En este proyecto, para describir las anotaciones usamos
un <strong><a href="http://www.w3.org/RDF/">RDF</a></strong> <a
href="http://www.w3.org/2000/10/annotation-ns">annotation schema</a>
especial.</p>

<p>Las anotaciones pueden guardarse localmente o en uno o m�s <b>servidores
de anotaciones</b>. Cuando un documento es hojeado, Amaya llama a cada uno de
esos servidores, solicitando las anotaciones relacionadas con el documento.
Actualmente Amaya presenta las anotaciones con los iconos de anotaci�n ( <img
src="../images/annot.png" alt="Icono l�piz de anotaci�n" /> ) que est�n
visualmente incrustados en el documento, como se muestra en la figura de
abajo. Si el usuario hace un �nico clic sobre un icono de anotaci�n, el texto
que ha sido anotado se resalta. Si el usuario hace doble clic sobre este
icono, el texto de la anotaci�n y otros metadatos se presentan en una ventana
aparte.</p>

<p style="text-align: center"><img src="../images/annotationicon-es.png"
alt="icono de anotaci�n (= l�piz)" /></p>

<p>Una anotaci�n tiene muchas propiedades, incluyendo:</p>
<ul>
  <li>Localizaci�n f�sica: �La anotaci�n est� almacenada en un archivo local
    o en una servidor de anotaciones?</li>
  <li>Alcance: �Est� la anotaci�n asociada a todo el documento o s�lo a un
    fragmento de ese documento?</li>
  <li>Tipo de anotaci�n: '<i xml:lang="en" lang="en">Anotaci�n</i>', '<i
    xml:lang="en" lang="en">Comentario</i>', '<i xml:lang="en"
    lang="en">Consulta</i>', ...</li>
</ul>

<h3 id="Local">Anotaciones locales y remotas</h3>

<p>Amaya puede guardar datos de anotaciones en un sistema de archivo local
(llamado "anotaciones locales") o en la Web (llamado "anotaciones remotas").
<b>Anotaciones remotas</b> son las almacenadas en servidores de anotaciones y
pueden ser descargadas y almacenadas por cualquiera que tenga los apropiados
derechos de acceso, tal como en el caso de otros documentos <acronym
title="HyperText Markup Language" xml:lang="en" lang="en">HTML</acronym>.
Tambi�n nos referimos a anotaciones remotas como <b>anotaciones p�blicas o
compartidas</b>, ya que pueden ser vistas por otras personas. Si deseas
instalar tu propio servidor de anotaciones, por favor, ve a <a
href="http://www.w3.org/1999/02/26-modules/User/Annotations-HOWTO.html">Annotation-Server-HOWTO</a>.</p>

<p>El usuario no necesita un servidor para hacer una <b>anotaci�n local</b>.
Las anotaciones locales se almacenan en el <a
href="/home/kahan/Amaya/doc/amaya/Configure.html">directorio de preferencias
de usuario</a>, en un directorio especial llamado "<code>annotations</code>",
y s�lo puede ser visto por el due�o (de acuerdo a c�mo est� organizado el
sistema de derechos de acceso). Este directorio est� conformado por tres
tipos de ficheros:</p>
<dl>
  <dt><strong>annot.index</strong></dt>
    <dd>asocia <acronym title="Uniform Resource Locator" xml:lang="en"
      lang="en">URLs</acronym> con los archivos en los que se almacenan los
      metadatos de las anotaciones.</dd>
  <dt><strong>index + random suffix</strong></dt>
    <dd>un fichero que almacena el metadato de las anotaciones relacionadas
      con una <acronym title="Uniform Resource Locator" xml:lang="en"
      lang="en">URL</acronym> dada. El metadato se especifica con RDF.</dd>
  <dt><strong>annot + random suffix.html</strong></dt>
    <dd>contiene el cuerpo de una anotaci�n, almacenado como <acronym
      title="EXtensible HTML" xml:lang="en" lang="en">XHTML</acronym>.</dd>
</dl>

<p>Un usuario puede, en cualquier momento, convertir una anotaci�n local en
una compartida usando la opci�n de men� <strong>Anotaciones/Post
anotaci�n</strong>. Si esta operaci�n tiene �xito, la anotaci�n local ser�
borrada ya que ser� movida a un servidor de anotacion. La siguiente secci�n
describe c�mo indicar el nombre del servidor en el que deber�an colocarse las
anotaciones.</p>

<h3 id="Annotation">Men� Anotaciones</h3>

<p>La mayor�a de los comandos necesarios para manejar anotaciones se
encuentran en el men� Anotaciones mostrado abajo. Los comandos se explican
m�s adelante en este documento, en el contexto de las metas del usuario
cuando maneja anotaciones.</p>

<p style="text-align: center"><img src="../images/annotationsmenu-es.png"
alt="Men� Anotaciones" /></p>

<h3 id="Configurat">Preparando la Configuraci�n para las Anotaciones</h3>

<p>El men� de configuraci�n de anotaciones se encuentra bajo la opci�n de
men� <strong>Anotaciones/Configurar</strong>. La siguiente figura muestra la
versi�n Windows de este men�. La versi�n Unix tiene una interfaz de usuario
un tanto diferente, pero las mismas funcionalidades.</p>

<p style="text-align: center"><img src="../images/configurebox-es.png"
alt="cuadro de di�logo de configuraci�n" /></p>

<p></p>
<dl>
  <dt><strong>Anotaci�n de usuario</strong></dt>
    <dd>Un campo que est� asociado con cada nueva anotaci�n como el
      <b>autor</b> de la anotaci�n. Amaya utiliza, por defecto, el
      identificador de usuario de la persona, cuando crea el campo de
      metadato autor. La selecci�n de la anotaci�n de usuario le permite a un
      usuario cambiar este nombre por uno m�s significativo, por ejemplo, en
      vez de <code>u1723242</code> (que hace fel�z a mi administrador de
      sistema) <code>marja</code> (que es el que usan otras personas
      normalmente cuando hablan conmigo).</dd>
  <dt><strong>Servidores de Anotaciones</strong></dt>
    <dd>Este campo le dice a Amaya qu� servidores debe contactar cuando
      busque anotaciones. Se pueden especificar uno o m�s servidores. Hemos
      reservado el nombre de servidor localhost para decirle a Amaya que
      queremos buscar anotaciones locales. Esto no significa que estemos
      poniendo en marcha un servidor local de anotaciones. Si este fuera el
      caso, necesitar�amos indicar la <acronym
      title="Uniform Resource Locator" xml:lang="en" lang="en">URL</acronym>
      completa. Bajo Unix, los servidores de anotaci�n se especifican como
      una lista separada por espacios. Bajo Windows, son a�adidos uno tras
      otro tecleando sus nombres directamente en el cuadro de di�logo,
      utilizando la tecla <code>Enter</code> para a�adir uno nuevo. Por
      defecto, el campo para servidores de anotaciones indica un
      <code>localhost</code>. 
      <p><em>Truco</em>: Si quieres deshabilitar temporalmente un servidor de
      anotaci�n, a�ade el car�cter "-" antes de su <acronym
      title="Uniform Resource Locator" xml:lang="en" lang="en">URL</acronym>.
      Amaya lo ignorar�.</p>
    </dd>
  <dt><strong>Autocargar anotaciones locales/Autocargar anotaciones
  remotas</strong></dt>
    <dd>Esta opci�n le dice a Amaya si solicita o no anotaciones,
      autom�ticamente (esto es, consultar a los servidores de anotaciones)
      cada vez que una <acronym title="Uniform Resource Locator"
      xml:lang="en" lang="en">URL</acronym> es navegada. Si no est�
      seleccionada, entonces el usuario tiene que invocar manualmente la
      funci�n <b>Cargar anotaciones</b> desde el men� <b>Anotaciones</b>.
      N�tese que si se selecciona la opci�n <code>autocargar anotaciones
      remotas</code>, puede haber un retraso mientras Amaya resuelve el
      nombre de DNS de los servidores de anotaci�n. Por defecto ambas
      opciones aparecen sin seleccionar.</dd>
  <dt><strong>Deshabilitar autocarga remota en cada inicio</strong></dt>
    <dd>Si esta opci�n est� seleccionada, Amaya restablecer� la opci�n
      <code>autocargar anotaciones remotas</code> cada vez que se inicie.
      Esta opci�n puede ser �til si est�s trabajando desconectado a ratos,
      pero seguir� queriendo autocargar las anotaciones locales, y las
      remotas de vez en cuando, por ejemplo, cuando te conectas durante un
      rato.</dd>
  <dt><strong>Servidor de publicaci�n</strong></dt>
    <dd><p>Este campo define el servidor en el que ser�n publicadas las
      anotaciones. Las anotaciones locales se guardan siempre en una base de
      datos local, independientemente del valor de este campo. Por defecto
      este campo est� vac�o. Advierte que el servidor de publicaci�n no est�
      incluido autom�ticamente en la lista de servidores a ser consultados,
      se debe indicar el nombre del servidor de publicaci�n en ambos campos,
      en cualquier orden que t� elijas.</p>
    </dd>
</dl>

<h3 id="Creating">Creando una anotaci�n</h3>

<p>Esta versi�n de Amaya soporta dos tipos de anotaciones: anotaciones que se
refieren a un documento completo y anotaciones que se refieren a un punto
espec�fico o selecci�n en un documento.</p>

<p>Para anotar un documento completo, tan solo selecciona la opci�n de
men�</p>

<h2></h2>

<p><b>Anotaciones/Anotar documento</b>. Para anotar un punto, pon el cursor
en cualquier punto en el documento y selecciona la opci�n de men�
<b>Anotaciones/Seleccionar anotaci�n</b>. Para anotar una selecci�n,
selecciona algo en el documento, y luego usa la opci�n de men�
<b>Anotaciones/Anotar selecci�n</b>. En todos estos casos aparecer� una
ventana de anotaci�n (ver la figura siguiente). El contenido de esta ventana
muestra el metadato de la anotaci�n, dentro de un recuadro, y el cuerpo de la
anotaci�n.</p>

<p style="text-align: center"><img src="../images/annotationwindow-es.png"
alt="cuadro de di�logo de configuraci�n de anotaciones" /></p>

<p></p>

<p>Normalmente, el metadato consiste en el <b>nombre del autor</b>, el
<b>t�tulo del documento anotado</b>, el <b>tipo de anotaci�n</b>, la <b>fecha
de creaci�n</b>, y la <b>fecha de �ltima modificaci�n</b>. Algunos de los
campos de metadatos tienen propiedades especiales. El campo <b>documento
fuente</b> es tambi�n un enlace que apunta de vuelta al texto anotado. Si el
usuario hace doble clic sobre �l, al igual que se sigue cualquier otro enlace
en Amaya, el documento anotado ser� obtenido y el texto anotado aparecer�
resaltado. El campo <b>tipos de anotaci�n</b> permite al usuario clasificar
la anotaci�n y cambiar su tipo. Haz doble clic sobre el texto "tipo de
anotaci�n" para ver la lista de tipos disponibles. Describiremos m�s abajo
c�mo pueden los usuarios definir tambi�n, sus propios tipos de anotaciones.
Finalmente, el campo <b>�ltima modificaci�n</b> se actualiza autom�ticamente
cada vez que se guarda una anotaci�n.</p>

<p>Debajo, el �rea principal es el <b>�rea del cuerpo de anotaci�n</b>.
Muestra el contenido actual y puede ser editada como si estuvi�ramos editando
cualquier otro documento <acronym title="HyperText Markup Language"
xml:lang="en" lang="en">HTML</acronym>, .N.B., aunque todav�a no se pueden
incluir im�genes en el cuerpo. Algunas de las caracter�sticas pueden no estar
listas en la versi�n inicial, tales como la vista Estructura.</p>

<p>Guardar una anotaci�n es equivalente a guardar cualquier otro documento en
Amaya. El usuario s�lo necesita seleccionar el comando <b>Archivo/Guardar</b>
(o utilizar su equivalente atajo de teclado o bot�n del men�). Las
anotaciones locales se guardan en el directorio "<span xml:lang="en"
lang="en">annotations</span>" y las anotaciones remotas se guardan en el
servidor de publicaci�n de anotaciones donde son almacenadas si el usuario ha
escrito el acceso. Para convertir una anotaci�n local en una compartida, el
usuario necesita usar el comando <b>Anotaciones/Publicar anotaci�n</b> y la
anotaci�n ser� guardada en el <b>Servidor de publicaci�n</b> que est�
definido en el men� de configuraci�n. Si esta operaci�n tiene �xito, la
anotaci�n local ser� borrada y las futuras operaciones de guardado ir�n
directamente al servidor de anotaciones. En la versi�n inicial, Amaya no
soporta una operaci�n de guardado de una copia de una anotaci�n remota en el
directorio local de anotaciones.</p>

<p>Algunos comandos aplicados al documento en la <b>ventana Documento
Amaya</b> se aplicar�n al documento de anotaci�n en la <b>ventana
Anotaci�n</b>. Por ejemplo, el cuerpo de una anotaci�n puede imprimirse con
el comando <b>Archivo/Imprimir</b> o recargado con el comando
<b>Archivo/Recargar documento</b>. (Nota: Algunas de estas caracter�sticas
pueden no estar inclu�das a�n).</p>

<h3 id="Deleting">Borrando una Anotaci�n</h3>

<p>El comando del men� Anotaciones/Borrar anotaci�n permite borrar una
anotaci�n. Se puede invocar este comando desde una ventana de anotaci�n
abierta. Tambi�n se puede borrar una anotaci�n desde el documento anotado
seleccionando primero la anotaci�n, haciendo un �nico clic sobre el icono de
anotaci�n, y luego llamando a este comando de men�.</p>

<h3 id="Loading">Cargando y Presentando Anotaciones</h3>

<p>El comando Cargar Anotaciones le dice a Amaya que cargue las anotaciones
asociadas a la <acronym title="Uniform Resource Locator" xml:lang="en"
lang="en">URL</acronym> del documento que est� siendo navegado en esa
ventana. Amaya consultar� al servidor de anotaciones, que est� indicado en el
cuadro de di�logo Anotaciones/Configurar, pidi�ndole cualquier anotaci�n
relevante.</p>

<p>Las anotaciones pueden descargarse tambi�n autom�ticamente siempre que se
carga una nueva p�gina seleccionando la casilla de verificaci�n <b>Autocargar
anotaciones</b> en el cuadro de di�logo <b>Anotaciones/Configurar</b>.</p>

<p>Notese que en esta versi�n de Amaya, al interrogar a un servidor de
anotaciones devolver� <i>todas</i> las anotaciones que est�n asociadas a un
documento. En una versi�n futura ser� posible usar un men� personalizado para
refinar la cadena de interrogantes que se env�a a los servidores.</p>

<p>El men� <b>Anotaciones/Filtro</b> local permite al usuario mostrar o
esconder los iconos de anotaci�n desde la vista de documento, por ejemplo,
para concentrarse en aquello que realmente le interesan o para hacer m�s
f�cil la lectura de un documento lleno de anotaciones. El usuario puede
mostrar o esconder anotaciones por tres tipos de metadatos: por el nombre de
<b>autor</b> de la anotaci�n, por el <b>tipo</b> de anotaci�n, y por el
nombre de <b>servidor de anotaci�n</b>. Para aplicar uno de estos filtros,
necesitas hacer clic en el bot�n de opci�n para seleccionar un tipo de
anotaci�n dado y, luego en el correspondiente bot�n de acci�n. Los comandos
<b>Mostrar todo</b> y <b>Esconder todo</b> se aplican a todas las
anotaciones. Un peque�o e inc�modo caracter a modo de prefijo se�ala el
estado de una entrada dada. Este caracter puede ser tanto un espacio (' '),
un asterisco ('*') o un gui�n ('-') para declarar que esas anotaciones que
pertenecen a esa entrada dada est�n todas visibles, todas escondidas,o
parcialmente visibles, respectivamente.</p>

<p>Ten en cuenta que el men� filtro s�lo muestra las anotaciones que conoce
que existen en el momento de su invocaci�n. Si a�ades nuevas anotaciones
entretanto, tendr�s que destruir este men� e invocarlo de nuevo para
verlas.</p>

<p>Advierte que para cada anotaci�n, el usuario de anotaci�n se muestra
concatenando el nombre del autor con el nombre del servidor de anotaci�n en
el que est� almacenada la anotaci�n, tal como se muestra en la siguiente
figura.</p>

<p></p>

<p style="text-align: center"><img src="../images/localfilter-es.png"
alt="cuadro de di�logo del filtro local" /></p>

<h3 id="Navigating">Navegando entre Anotaciones</h3>

<p>La anotaciones aparecen como enlaces en la <b>ventana Enlaces</b> que
puede abrirse mediante el comando <b>Vistas/Mostrar enlaces</b>. En esta
ventana las anotaciones aparecen marcadas con el mismo icono del l�piz, usado
en las anotaciones en la ventana de documento. La ventana Enlace muestra
todas las anotaciones, sin tener en cuenta si han sido escondidas con el men�
<b>Filtro Local <span
style="font-weight: normal; font-family: times">de</span> Anotaciones</b>. Al
igual que en la ventana de documento, un �nico clic sobre la anotaci�n
seleccionar� el texto anotado en la ventana de documento y un doble clic
abrir� la anotaci�n. Este es un ejemplo de c�mo navegar de un enlace de
anotaci�n a otro, incluso si la anotaci�n no pude ser vista por el usuario
debido a su discapacidad o debido a las caracter�sticas del dispositivo
usado.</p>

<p></p>

<p style="text-align: center"><img src="../images/linkwindow-es.png"
alt="La vista enlaces muestra las anotaciones" /></p>

<h3 id="Moving">Moviendo anotaciones</h3>

<p>Amaya tambi�n te permite mover una anotaci�n a cualquier otra parte del
mismo documento. Esto es interesante, por ejemplo, para manejar <a
href="#Issues1">anotaciones hu�rfanas y corruptas</a>. Se puede mover una
anotaci�n a la selecci�n actual o tambi�n a el valor de un XPointer
almacenado. Por el momento s�lo es posible mover anotaciones en el mismo
documento en el que han sido creadas.</p>

<p>Para mover una anotaci�n a la selecci�n actual, primero hay que abrir la
ventana de anotaci�n. Entonces hay que hacer clic en el documento anotado y
seleccionar algo en �l. Luego, en la Ventana Anotaci�n, invocar el men�
<b>Anotaciones/Mover a selecci�n</b>. Amaya mover� entonces el icono de
anotaci�n al texto seleccionado y marcar� el documento de anotaci�n como
modificado. Para conseguir que este cambio se efectivo hay que guardar la
anotaci�n, de otra manera se perder�n los cambios. Al igual que en la
creaci�n de anotaciones se puede mover una anotaci�n a la posici�n actual del
cursor, sin tener que hacer una selecci�n.</p>

<p>Tambi�n se puede memorizar la posici�n a la que se quiere mover la
anotaci�n y, entonces, moverla ah�. Esto es �til, por ejemplo, si se quieren
mover varias anotaciones a la misma posici�n o si se quiere desplazarse a
cualquier parte del documento antes de moverlas. Para hacer esto, haz una
selecci�n (o s�lo posiciona el cursor) en el lugar en el que quieres mover la
anotaci�n. Entonces utiliza el men� <b>Enlace/Almacenar enlace como
XPointer</b> para almacenar internamente un XPointer que represente la
selecci�n. Finalmente, en la ventana Anotaciones, utiliza el men�
<b>Anotaciones/Mover para almacenar XPointer</b> para mover la anotaci�n a su
nueva localizaci�n. Al igual que antes deber�s guardar la anotaci�n para
hacer efectivo el cambio.</p>
</div>

<div id="Issues">
<h3 id="Issues1">Problemas con Anotaciones y Documentos Modificados</h3>

<p>Si se est� usando anotaciones con documentos en vivo (documentos cuyos
contenidos pueden ser modificados) puedes encontrar dos tipos de problemas:
<b>anotaciones hu�rfanas</b> y <b>anotaciones corruptas</b>. D�janos antes
describir un poco m�s en detalle c�mo Amaya a�ade anotaciones a los
documentos.</p>

<p>Amaya utiliza <strong><a
href="http://www.w3.org/XML/Linking">XPointer</a></strong> para describir
d�nde debe a�adirse una anotaci�n a un documento. XPointer est� basado en la
estructura del documento. Para construir un XPointer, por ejemplo para una
selecci�n, comenzamos desde el primer punto de la selecci�n y vamos
regresando a trav�s de la estructura del documento hasta encontrar la ruta
del documento. Si un elemento tiene un atributo ID, paramos de construir el
XPointer y consideramos que el comienzo de ese XPointer es el elemento que
tiene ese valor de atributo ID. Por ejemplo, si miras el c�digo <acronym
title="HyperText Markup Language" xml:lang="en" lang="en">HTML</acronym> de
este documento, notar�s que esta secci�n (Problemas con anotaciones...) est�
incluida dentro de un elemento DIV que tiene un atributo ID con el valor
"Issues". Aqu� hay un extracto del c�digo fuente:</p>
<pre>  &lt;div id="Issues"&gt;
  &lt;h3&gt;Problemas con ....&lt;/h3&gt;
  &lt;p&gt;Si se est� usando...&lt;/p&gt;
  &lt;p&gt;Amaya utiliza &lt;strong&gt;XPointer&lt;/strong&gt;...&lt;/p&gt;
  ...
  &lt;/div&gt;</pre>

<p>Y aqu� el XPointer que apunta al segundo p�rrafo:</p>

<p style="text-align: center"><code>xpointer(id("Issues")/p[2])</code></p>

<p>El XPointer de arriba apunta al segundo elemento p, desde el elemento
padre que tiene un atributo ID con el valor "Issues". (Para seleccionar un
p�rrafo completo, ponemos el cursor en el p�rrafo y presionamos la tecla
escape [Unix] o F2 [Windows]). N�tese que el uso del atributo ID permite al
autor del documento mover la selecci�n completa, donde quiera en el
documento, sin necesidad de actualizar el XPointer. El XPointer no depende
del elemento que precede ese particular elemento DIV.</p>

<p>Decimos que una anotaci�n se queda <b>hu�rfana</b> cuando ya no puede ser
a�adida a un documento, esto es, cuando el XPointer no determina nunca m�s
ning�n elemento en la estructura. Esto pasa cuando una estructura de
documento se modifica. Por ejemplo, el XPointer de arriba no podr�a
determinar nunca m�s si eliminamos el atributo ID "Issues". Amaya te
advertir� si detecta cualquier anotaci�n hu�rfana mientras descarga una serie
de anotaciones desde un servidor de anotaciones. Todas las anotaciones
hu�rfanas son visibles desde la vista Enlaces y est�n asociadas con un icono
que muestra un signo de interrogaci�n superpuesto al l�piz de anotaciones
<img src="../images/annotorp.png" alt="icono de anotaciones hu�rfanas" />.</p>

<p>Decimos que una anotaci�n es <b>corrupta</b> cuando apunta a una pieza de
informaci�n err�nea. Volviendo a nuestro ejemplo, podemos crear una anotaci�n
corrupta si intercambiamos el primer y segundo p�rrafo. El XPointer
continuar� apuntando al segundo p�rrafo, a pesar de que nos gustar�a que
apuntara al primer p�rrafo. Este problema se hace incluso m�s com�n cuando
anotas una porci�n de texto que puede cambiar. En la primera versi�n, Amaya
no advierte al usuario si una anotaci�n est� corrupta. Una futura versi�n
puede avisar al usuario de la posibilidad de que una anotaci�n pueda llegar a
ser corrupta.</p>

<p>�C�mo puedes protegerte a ti mismo?</p>

<p>Si eres el autor de un documento, intenta usar el atributo ID en lugares
estrat�gicos, por ejemplo, en los elementos DIV. Amaya te permite asociar
autom�ticamente o eliminar un atributo ID a/desde una serie de elementos por
medio del comando de men� Especial/Crear/Suprimir atributos ID. En el ejemplo
de arriba, nosotros podr�amos haber evitado el problema de las anotaciones
corruptas si hubi�semos a�adido un atributo ID al segundo p�rrafo:</p>
<pre>  &lt;p id="Amaya"&gt;Amaya utiliza...&lt;/p&gt;</pre>

<p>Un XPointer que apunta a este p�rrafo es:</p>

<p style="text-align: center"><code>xpointer(id("Amaya"))</code></p>

<p>De esta manera, el XPointer apuntar� al mismo p�rrafo, independientemente
de su posici�n en la estructura del documento.</p>
</div>

<p><a href="MakeBook.html"><img alt="previa" src="../images/left.gif" /></a>
<a href="Manual.html"><img alt="arriba" src="../images/up.gif" /></a> <a
href="Configure.html"><img alt="siguiente" src="../images/right.gif"
/></a></p>
<hr />
</body>
</html>

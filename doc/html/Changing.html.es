<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <title>Cambiando la estructura</title>
  <meta name="GENERATOR" content="amaya 5.2" />
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
</head>

<body xml:lang="es" lang="es">

<table border="0" summary="toc">
  <tbody>
    <tr>
      <td><p><img alt="W3C" src="../images/w3c_home" /> <img alt="Amaya"
        src="../images/amaya.gif" /></p>
      </td>
      <td><p><a href="Links.html" accesskey="p"><img alt="previa"
        src="../images/left.gif" /></a> <a href="Manual.html"
        accesskey="t"><img alt="arriba" src="../images/up.gif" /></a> <a
        href="Tables.html" accesskey="n"><img alt="siguiente"
        src="../images/right.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="Changing">
<h2>Editando la estructura de documento</h2>

<h3><a name="L653">Cambiando el tipo de elemento</a></h3>

<p>Para cambiar el tipo de un elemento existente, selecci�nalo y elige el
nuevo tipo en el men� <strong>Tipos</strong> o haciendo clic en un bot�n.
Entonces puedes cambiar una lista numerada en una lista con vi�etas, por
ejemplo, o un p�rrafo en un encabezado o lista. Para eliminar un tipo que ha
sido seleccionado desde el men� <strong>Estilo</strong> (<a
href="HTML-elements/inline.html">tipos en l�nea</a> - aquellos que pueden ser
una frase en un p�rrafo o lista) utiliza la opci�n apropiada en el submen�
del men� Estilo como un conmutador. La mayor�a de estos tipos pueden ser
eliminados para transformar el elemento cuando han sido seleccionados (ver
m�s abajo).</p>

<p>Para seleccionar un elemento completo, coloca el punto de inserci�n en
cualquier lugar dentro de �l y presiona la tecla <a
href="Selecting.html#Selecting1">Esc (F2 en Windows)</a>.</p>

<p>Tambi�n es posible hacer <a href="#L631">transformaciones de
estructura</a> m�s complejas.</p>

<h3><a name="L631">Cambiando la estructura de documento</a></h3>

<p>Pueden conseguirse transformaciones simples en la estructura utilizando el
men� <b>Tipos</b> (ver <a href="#L653">arriba</a>). Se pueden conseguir
tranformaciones m�s complejas por otros medios.</p>

<h3><a name="L141">El comando Transformar</a></h3>

<p>El comando <b>Tranformar</b> del men� <b>Editar</b> lleva a cabo complejas
transformaciones de la estructura. Este comando se basa en un lenguaje de
transformaci�n de estructura que especifica c�mo puede ser transformado un
patr�n estructural dado en diferentes estructuras.</p>

<p>El comando <b>Transformar</b> saca un men� que lista todas las
transformaciones que pueden aplicarse a la selecci�n actual. S�lo hay que
elegir la transformaci�n que se quiere.</p>

<p>Las transformaciones disponibles est�n definidas en el archivo
<code>Amaya/amaya/HTML.trans</code>. V�ase el documento <a
href="Transform.html">Uso del archivo HTML.trans</a> para ver una descripci�n
del lenguaje en el que est�n escritas las normas de transformaci�n.</p>

<p>Pueden especificarse transformaciones adicionales en el archivo
<code>HTML.trans</code>. Algunas se proporcionan como ejemplo. Por ejemplo,
es posible transformar listas anidadas en una tabla, y viceversa.</p>

<h3><a name="Inserting">Insertando una Divisi�n</a></h3>

<p>Se puede agrupar f�cilmente una serie de p�rrafos, encabezados, listas,
tablas, etc.; dentro de una divisi�n. Para hacerlo hay que seleccionar el
elemento a ser agrupado y seleccionar la entrada DIV en el men� Tipos.</p>

<h3><a name="Back">Las teclas Borrar y Suprimir</a></h3>

<p>Pueden combinarse elementos sucesivos, incluso si no est�n al mismo nivel
en la estructura de documento. La combinaci�n se consigue con las teclas
Borrar y Suprimir. Cuando el punto de inserci�n est� al final de un elemento,
utiliza la tecla Suprimir, cuando est� al principio, utiliza la tecla Borrar.
En ambos casos, estas teclas no borrar�n ning�n car�cter, pero combinar�n los
elementos que est�n separados por un l�mite de estructura.</p>

<p>Cuando el punto de inserci�n est� dentro de un elemento, estas dos teclas
borran el car�cter siguiente o previo, como de costumbre.</p>
</div>

<p><a href="Links.html"><img alt="previa" src="../images/left.gif" /></a> <a
href="Manual.html"><img alt="arriba" src="../images/up.gif" /></a> <a
href="Tables.html"><img alt="siguiente" src="../images/right.gif" /></a></p>
<hr />
</body>
</html>

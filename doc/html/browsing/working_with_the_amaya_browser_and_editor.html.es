<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta name="GENERATOR"
  content="amaya 8.0-pre, see http://www.w3.org/Amaya/" />
  <title>Trabajar con el Navegador y Editor Amaya</title>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="es" lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="../Browsing.html.es" accesskey="t"><img
        alt="superior" src="../../images/up.gif" /></a> <a
        href="opening_documents.html.es" accesskey="n"><img alt="siguiente"
        src="../../images/right.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Trabajar con el Navegador y Editor Amaya</h1>

<p>Con Amaya puedes <a href="opening_documents.html.es">abrir</a> y navegar
documentos Web locales y remotos. Tambi�n puedes crear y modificar p�ginas
Web.</p>

<h2>Navegaci�n frente a edici�n</h2>

<p>Navegar con Amaya es como navegar con otros navegadores Web. Por ejemplo,
la barra de botones de Amaya incluye botones stop, atr�s, adelante,
actualizar, p�gina de inicio, guardar e imprimir. Pero hay una diferencia
importante: para abrir un enlace debes hacer doble clic sobre �l. Consulta <a
href="activating_a_link.html.es">Activar un enlace</a> para obtener m�s
informaci�n.</p>

<p>A diferencia de otros navegadores, Amaya proporciona herramientas para la
edici�n de p�ginas web. Para obtener m�s informaci�n sobre la edici�n,
consulta:</p>
<ul>
  <li><a href="../Changing.html.es">Editar Documentos</a></li>
  <li><a href="../Tables.html.es#page_body">Editar Tablas</a></li>
  <li><a href="../Math.html.es#page_body">Editar MathML</a></li>
</ul>

<h2>Modo editor y Modo navegador</h2>

<p>Por omisi�n, Amaya empieza en modo editor. En este modo puedes editar y
navegar el documento en la misma ventana.</p>

<p>La barra de botones muestra el bot�n <img src="../../images/Editor.gif"
alt="Bot�n Editor" /> cuando la ventana est� en modo editor, y el bot�n <img
src="../../images/Browser.gif" alt="Bot�n navegador" /> cuando la ventana
est� en modo navegador. En modo navegador solamente puedes navegar y rellenar
formularios.</p>

<p>Para pasar del modo editor al modo navegador, haz clic en el bot�n <img
src="../../images/Editor.gif" alt="Bot�n Editor" />. Haz clic en el bot�n
<img src="../../images/Browser.gif" alt="Bot�n navegador" /> para volver al
modo editor. Tambi�n puedes elegir el modo con el comando de men�
<strong>Editar &gt; Modo Edici�n</strong>, o con un atajo de teclado (el
atajo por omisi�n es <kbd>Shift-Control-*</kbd>).</p>

<p class="Note"><strong>Nota:</strong> El modo se puede elegir de forma
independiente en cada ventana de documento.</p>

<h2>Juegos de caracteres</h2>

<p>Amaya lee los documentos HTML and XHTML de forma distinta:</p>
<ul>
  <li>Amaya lee por omisi�n los documentos HTML como archivos <a
    href="../editing_iso-latin-1_characters/entering_ISOLatin1_characters_in_amaya.html.es">ISO-Latin-1</a>.
    Si el autor no ha incluido informaci�n sobre el juego de caracteres en el
    encabezado HTTP o en un elemento META, Amaya considera que el documento
    HTML est� codificado en ISO-Latin-1.</li>
  <li>Amaya lee por omisi�n los documentos XHTML como archivos UTF-8. Si el
    autor no ha incluido informaci�n sobre el juego de caracteres en el
    encabezado HTTP, en la declaraci�n XML o en un elemento META, Amaya
    considera que el documento XHTML est� codificado en UTF-8.</li>
</ul>

<p>Puesto que el juego de caracteres es distinto en los documentos HTML y
XHTML, los autores a menudo crean documentos con un juego de caracteres
incorrecto. Por ejemplo, imagina un documento XHTML que utiliza el juego de
caracteres ISO-Latin-1 pero no incluye informaci�n sobre el juego de
caracteres utilizado. Cuando el analizador XML analiza el documento y
encuentra un caracter que no es UTF-8, el documento se considera
gramaticalmente incorrecto. El an�lisis se detiene y Amaya muestra un mensaje
de error que propone abrir el documento como documento HTML o bien mostrar
los errores encontrados en el an�lisis. Si eliges mostrar los errores, Amaya
ejecuta el comando de men� <strong>Archivo &gt; Reanalizar como
HTML</strong>. Elige este comando para definir el juego de caracteres como
ISO-Latin-1 y reabrir el documento de manera que pueda ser analizado
adecuadamente. Amaya considera que el documento se ha modificado (ya que has
a�adido un juego de caracteres) y puedes guardarlo.</p>
</div>
</body>
</html>

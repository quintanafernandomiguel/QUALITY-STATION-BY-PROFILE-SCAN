
![image](https://github.com/user-attachments/assets/06f46917-8bc7-4f13-b25f-528d409aa307)
![image](https://github.com/user-attachments/assets/6d13bf0c-cc66-49a6-a1e0-0788450a18fb)


**Título del Proyecto
“Quality station by profile scan”

Motivación y/o Antecedentes que Dan Origen a la Propuesta
En la industria moderna, donde la velocidad, precisión y repetibilidad son esenciales en los procesos de fabricación, asegurar la calidad dimensional de cada pieza producida es un requisito clave. A pesar de los avances tecnológicos, muchas líneas de producción aún dependen de inspecciones manuales, que no solo ralentizan el proceso, sino que también están expuestas a errores humanos y limitaciones en la trazabilidad de los datos obtenidos.
Frente a este escenario, surge la necesidad de automatizar el control de calidad, buscando sistemas que puedan operar de forma eficiente, precisa y continua, sin intervención humana. En este contexto, nació mi interés personal por el funcionamiento de los perfilómetros industriales, dispositivos capaces de captar con gran exactitud el perfil de una pieza.
A partir de esa curiosidad, exploré las posibilidades de replicar esta tecnología utilizando herramientas más accesibles, como una cámara web combinada con un láser de línea, y aprovechar el potencial de procesamiento de imágenes con Python, OpenCV y técnicas de triangulación láser.
Este proyecto no solo busca responder a una necesidad real de la industria, sino también explorar una alternativa funcional y didáctica al uso de perfilómetros industriales de alto costo. La estación desarrollada permitirá escanear perfiles en tiempo real, evaluar su conformidad y tomar decisiones automáticas sobre el descarte de piezas fuera de especificación, integrando así tecnologías de visión, control automatizado y mecatrónica aplicada.







Descripción de la Propuesta
El proyecto consiste en el desarrollo de una estación de control de calidad automatizada que permite inspeccionar piezas en tiempo real a través de tecnología de escaneo de perfiles. El objetivo principal es evaluar si cada pieza cumple con los estándares dimensionales definidos y, en caso contrario, descartarla automáticamente del proceso productivo.

El sistema está compuesto por los siguientes elementos principales:

•	Una cinta transportadora motorizada, encargada de trasladar las piezas a través de la estación de inspección.
•	Un perfilómetro casero, diseñado a partir de una cámara web y un láser de línea, que mediante triangulación láser y procesamiento de imágenes con Python y OpenCV, permite reconstruir el perfil de cada objeto inspeccionado.
•	Un sensor láser, que mide la distancia a lo largo del recorrido de la pieza para complementar el análisis dimensional.
•	Un sistema de control basado en Arduino, encargado de gestionar la adquisición de datos, el análisis de los mismos y control de todos los actuadores de la cinta.
•	Un mecanismo de rechazo mediante servoaccionamiento, que actúa automáticamente cuando se detecta una pieza fuera de tolerancia, desviándola de la línea principal.
•	Una interfaz gráfica sencilla, desarrollada en Jupyter Notebook, que permite visualizar en tiempo real el estado del sistema, los perfiles analizados y los resultados de la evaluación.

Esta propuesta combina distintas áreas de la ingeniería mecatrónica, como el diseño mecánico, la automatización, el control electrónico y la visión artificial, con el objetivo de ofrecer una solución funcional, accesible y replicable.

Además, permite experimentar con conceptos avanzados como el escaneo láser 2D/3D y el procesamiento de imágenes, aplicados a una problemática real de la industria: la necesidad de inspección continua y confiable, sin intervención humana directa.



Alcance del Proyecto
El proyecto tiene como objetivo presentar una estación de control de calidad totalmente funcional, integrando tanto el desarrollo mecánico como electrónico y de software. Se busca lograr una implementación completa, donde todos los subsistemas trabajen de forma coordinada para inspeccionar, evaluar y tomar decisiones automáticas sobre las piezas en una línea de producción.

Los principales componentes y funcionalidades que se desarrollarán son:

•	Diseño CAD detallado de toda la estación, incluyendo la estructura de soporte, la cinta transportadora, y el mecanismo de descarte.

•	Fabricación e implementación de la cinta transportadora, traccionada por un motor paso a paso, controlado desde una placa Arduino.

•	Integración de un sistema de escaneo con perfilómetro desarrollado a partir de una cámara web y un láser de línea, utilizando triangulación láser para obtener el perfil de las piezas.

•	Procesamiento de imágenes y reconstrucción del perfil en tiempo real, mediante herramientas como Python y OpenCV.

•	Desarrollo de la lógica de control y análisis en Arduino para gestionar la adquisición de datos, comparación con un perfil de referencia y activación del sistema de descarte.

•	Implementación de un servoaccionamiento que permita ejecutar el rechazo de piezas fuera de especificación mediante un mecanismo físico.

•	Interfaz de usuario básica, programada en Jupyter Notebook, que muestre gráficamente el estado del sistema, los perfiles escaneados y los resultados de validación.


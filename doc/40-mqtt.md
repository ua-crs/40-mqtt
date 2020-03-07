

![Austral Ingenieria](https://encrypted-tbn0.gstatic.com/images?q=tbn%3AANd9GcQooGo7vQn4t9-6Bt46qZF-UY4_QFpYOeh7kVWzwpr_lbLr5wka)


# Proyecto 40-mqtt


## Secciones

- [Objetivo](#objetivo)
- [Programa](#programa)
- [platformio.ini](#platformio.ini)
- [Constantes](#constantes)
- [Bibliotecas](#bibliotecas)

___

## Objetivo

>   El objetivo es comunicar dos plaquetas mediante MQTT.

>   El conjunto permitirá lo siguiente;
>   - Hacer que la posición del potenciómetro en una de las placas cambie la posición del servo en la otra
>   - Hacer que la opresión de un pulsador en una placa, cambie el estado de un LED en la otra

### Medios utilizados

-   Las plaquetas base que pueden ser utilizadas son las siguientes:
    -   Wemos D1 R2 (con _ESP8266_ )
    -   Wemos D1 R32 (con _ESP32_ )
-   Sobre la plaqueta base, se coloca un _shield_ que es el utilizado en IAE
-
-   Para conveniencia de reconocimiento, se sugiere rotular a cada conjunto de base y _shield_ con los nombres _G1_ y _G2_
-
-   Se utilizarán servidores de MQTT para la prueba de dos tipos:
    -   Servidor propio: para ello, se sugiere usar el servidor _mosquitto_ en Linux
        - Se puede instalar sobre una máquina Linux o sobre Raspberry Pi corriendo _Raspbian_
    -   Servidor en el Web
        - En el ejemplo, se ha utilizado [ClodMQTT](https://www.cloudmqtt.com/)

____

### Programa

El programa está dividido en cinco archivos, a saber:
>   _main.cpp_: programa principal, independiente del _hardware_

>   _hard.h_: archivo de inclusión con el manejo específico de _hardware_

>   _defs.h_: definiciones (aprte de las que están en _platformio.ini_.

>   _wifi_ruts.cpp_: rutinas de manejo de WiFi

>   _wifi_ruts.h_: prototipos de las rutinas de manejo de WiFi

- Archivo ___main.cpp___

    - Entre las líneas 5 y 18, se colocan las inclusiones de archivos, tanto los del sistema, como de las bibliotecas y los propios; en este último caso, se trata de _wifi_ruts.h_ y de _defs.h_
    - Entre las líneas 21 y 35, mediante compilación condicional determinada por el símbolo _MQTT_, se elige entre el servidor en el Web (_MQTT_ == 1) y el servidor local (_MQTT_ == 2)
    - Entre las líneas 42 y 46, se define un nuevo tipo de dato que permitirá almacenar duplas formadas por un subtópico y el mensaje asociado: dicho tipo se denomina _topic\_t_
    - En las líneas 48 y 49 se instancian dos objetos del tipo _WiFiClient_ y _PubSubClient_
    - En la línea 51 se define un arreglo de caracteres _client\_id_ que permite alojar nombre de cada cliente que se conecte a MQTT
    - Finalmente, en la línea 53 se incluye el archivo _hard.h_ que posee todo el código dependiente del _hardware_.
        - Debe aclararse que es una práctica no muy común colocar código ejecutable en un archivo _header_ pero se lo ha hecho para ser más fácil para el neófito.
        - Por lo tanto, se debería seguir leyendo por ese archivo, tal cual lo hace el compilador; sin embargo, por unidad de explicación se seguirá por el archhivo _main.cpp_
    - A partir de la linea 55 en adelante, se encuentra el código _independiente_ del _hardware_
    - Función _callback_
        - Cada vez que el _broker_ MQTT informa que hubo un cambio en un tópico al cual la estación está suscripta, llama a esta función, con el tópico _topic_ y el mensaje _message_.
        - Para todas las duplas contenidas en el arreglo _topics_, se busca si coincide el tópico recibido con alguno de los allí almacenados y, si es así, se ejecuta la acción almacenada en la dupla.
    - Función _init_mqtt_
        - A través del cliente de MQTT creado en la línea 49 con el nombre _client_, se intenta conectar con el servidor elegido.
        - Para ello, en la línea 76 se indica la dirección de Internet donde se encuentra el servidor y el puerto de acceso, en la línea 77 se registra en el cliente la dirección de la rutina de _callback_.
        - Comenzando en la línea 78, se comienza un ciclo de intento de conexión; en efecto, en la línea 82 se invoca el método del cliente denominado _connect_ a quien se le pasa la identificación de este cliente, el usuario y la clave de conexión.
    - Función _setup_
        - A partir de la línea 97, se realizan las siguientes acciones
            - Obtener el _string_ de identificación de este cliente.
            - Inicializar la comunicación serie
            - Conectarse a WiFi
            - Inicializar el _hardware_
            - Inicializar las suscripciones deseadas en el servidor
            - Prender el led de la placa en señal de conexión Ok.
    - Función _loop_
        - Llamar a la función _verify_changes_ para determinar si hubo cambios en el _hardware_ que haya que publicar en el servidor.
        - Llamar al método de _client_ denominado _loop_ que verificará si hay novedades de las suscripciones deseadas y, en caso afirmativo, llamar a la función de _callback_ oportunamente registrada.


- Archivo ___hard.h___
    - Como se dijo anteriormente, este archivo es absolutamente dependiente de _hardware_ y, en este caso particular, manejará los siguientes periféricos
        - LED
        - Pulsador
        - Potenciómetro
        - Servo motor
    - En la línea 5 se crea un obeto del tipo _Servo_
    - Función _get_client_id_
        - Las líneas 7 a 11 se utilizan para generar la identificación de cada cliente para el servidor.
    - Función _init_hardware_
        - Las líneas comprendidas entre 18 a 27 se realiza la inicialización de todo el _hardware_, a saber:
            - LED como salida
            - Pulsador como entrada, con resistencia de _pullup_
            - Apagar el LED
            - Anexar al objeto servo el _pin_ de servicio del servo
            - Inicializar la primera posición del servo en el ángulo 0
    - Función _led\_setup\_done
        - Se cambia el estado del LED

    - Función _subscribe\_to_
        - Recibe el subtópico
        - Arma el _string_ de tópico en el formato _tópico_base/grupo/subtópico_; por ejemplo sería _espxx/g1/toggle_ o _espxx/g2/angle_.
        - Envía el _string_ de suscripción al servidor.

    - Función _init\_suscriptions_
        - Se suscribe, a través de la función anterior, a los subtópicos _toggle_ y _angle_

    - Función _change\_led_
        - Acción relacionada con la suscripción a subtópico _toggle_
        - Recibe el mensaje (_msg_) del servidor pero no tiene efecto en la acción.
        - Cambia (_toggle_) el estado del led

    - Función _write\_servo_
        - Acción relacionada al subtópico _angle_
        - Recibe como mensaje (_msg_) del servidor el ángulo a girar como texto
        - Envía la orden de girar al servo del ángulo enviado como mensaje previamente convirtiendo a un número entero.

    - Arreglo _topics_ de estructuras tipo _topic\_t_
        - Agrupa subtópico y acción a realizar

    - Función _do\_publish_
        - Recibe el subtópico y el mensaje
        - Arma el _string_ de tópico
        - Lo publica en el servidor

    - Entre las líneas 103 a 107 se definen variables globales para la observación del pulsador

    - Función _verify\_pushbutton_
        - Esta función trata de encontrar el cierre del pulsador como transición efectiva para informar al servidor (la de apertura se debe detectar pero no se informa).
        - De todas maneras, para tomar dichas transiciones se deben evitar los típicos rebotes (_bounce_) de cierre o apertura de contactos a través de un temporizador que, en este caso es de 50 mseg (línea 107).
        - La decisión de publicación se encuentra en las líneas 126 y 127

    - Entre las líneas 136 y 139 se crean e inicializan las variables globales necesarias para la función _verify\_pote_.

    - Función _verify\_pote_
        - Esta función verifica el valor de posición del potenciómetro.
        - El potenciómetro está conectado a 3.3v y su punto medio está conectado al conversor A/D _ANAIN_.
        - Se realiza la medición cada 100 mseg., lo cual está dado por la línea 138.
        - La medición del conversor A/D será entre 0 y RANGE, de acuerdo a la cantidad de bits del conversor.
        - El valor leído se convierte de 0 a RANGE a 0 a 180 mediante la función _map_.
        - Este valor de ángulo tendrá una resolución de _skip_angle_.
        - Si en estas condiciones, este valor de ángulo es distinto al del muestreo anterior, entonces se publica en el servidor.

    - Función _verify\_changes_
        - Se llama sucesivamente a:
            - _verify\_pushbutton_
            - _verify\_pote_

- Archivo ___defs.h__
    - En este archivo, se colocan algunas definiciones.
    - La mayor parte de las definiciones están en _platformio.ini_
    - Entre las líneas 14 y 20, está como se arma el grupo dentro del tópico.
    - Como las acciones de ambas placas son simétricas, entonces aquí lo que se decide es quien es _ME_ y _YOU_.
    - Si la placa es la G1, entonces se deberá poner en _platformio.ini_ como _BOARD_ el valor 1, sino se deberá colocar 2.
    - 
    - Entre las líneas 31 y 33 están los strings correspondientes al tópico principal (_espxx_ en nuestro caso) y como es el grupo en el tópico.

- Archivo ___wifi\_ruts.cpp___
    - Códigos referidos al manejo de WiFi

- Archivo ___wifi\_ruts.h___
    - Archivo de inclusión con los prototipos de las funciones del anterior

____

#### platformio.ini

<!--
    Aquí explicación sobre la configuración en platformio.ini
-->

> ___NOTA IMPORTANTE___:

> En el caso de la placa con ESP32, no funciona adecuadamente la obtención del posicionamiento del potenciómetro. Ello se debe a que la placa de _shield_ IAE fué hecha para trabajar con _ESP8266_ y en éste sólo existe un conversor A/D que se conecta en la posición ___0___ del la fila de conectores de analógicos. En el caso del _ESP32_, este contacto corresponde al conversor ADC2 que no funciona si se usa simultáneamente WiFi

### Constantes

<!--
    Constantes usadas en el programa y su definición
-->

>   A continuación, se explican las constantes que se definen en _platformio.ini_

- ___[platformio]___
    - Aquí se colocan cuál o cuales son los ambientes que se compilarán

- ___[env]___
    - Ambiente con definiciones comunes a todos los demás ambientes
    - ___framework___
        - Todos los ambientes usan el _framework_ de _arduino_
    - ___build\_flags___
        - _MOSQUITTO\_IP_: _string_ con el número de IP del servidor de _mosquitto_ cuando es local
        - _MY\_SSID_: _string_ con el nombre de AP para conectarse a WiFi
        - _MY\_PASS_: _string_ con la clave de WiFi
        - _BOARD_: 1 para la placa _G1_, 2 para la placa _G2_
        - _MQTT_: 1 para el servidor en el Web, 2 para el servidor local
- ___[env:wemos\_d1\_mini32]___
    - Aquí se colocan las definiciones válidas para _ESP32_
    - _monitor\_speed_: baud rate de conexión de la PC a la placa
    - ___build\_flags___
        - Aquellas definidas en ___[env]___
        - _PIN\_BUTTON_:    IOport donde está conectado el pulsador
        - _PIN\_LED_:       IOPort donde está conectado el LED
        - _PIN\_SERVO_:     IOPort donde está conectado el Servo
        - _ANAIN_:          IOPort de acceso al conversor AD
        - _RANGE_:          Rango del conversor AD
        - _SERIAL_BAUD:     Baud rate del port serie (tomado de _monitor\_speed_)
- ___[env:d1_mini]___
    - Aquí se colocan las definiciones válidas para _ESP8266_
    - _monitor\_speed_: baud rate de conexión de la PC a la placa
    - ___build\_flags___
        - Aquellas definidas en ___[env]___
        - _PIN\_BUTTON_:    IOport donde está conectado el pulsador
        - _PIN\_LED_:       IOPort donde está conectado el LED
        - _PIN\_SERVO_:     IOPort donde está conectado el Servo
        - _ANAIN_:          IOPort de acceso al conversor AD
        - _RANGE_:          Rango del conversor AD
        - _SERIAL_BAUD:     Baud rate del port serie (tomado de _monitor\_speed_)
 
____

#### Bibliotecas

<!--
    Uso y configuración de bibliotecas
-->

- ___[env]___
    - ___lib\_deps___
        - Nombre de la/s biblioteca/s usada/s por todos los ambientes
        - En este caso, el nombre es _PubSubClient_
        - A pesar que se refiere que es solamente utilizable por _ESP8266__ se puede usar también con _ESP32_

- ___[env:wemos\_d1\_mini32]___
    - ___lib\_deps___
        - Aquellas definidas en ___[env]___
        - _ServoESP32_: para el servo, especial para _ESP32_

- ___[env:d1_mini]___
    - No existen bibliotecas particulares para _ESP8266_
    - Sólo se requiere la ya definida en ___[env]___ y aquellas que acompañan por _default_ a _Arduino_ (entre ellas la biblioteca _Servo_)



/*
 *  Hardware dependent code
 */

Servo servo;

void
get_client_id( char *id )
{
    sprintf( id, "%s_%s", client_name, group );
}


/*
 *  hardware initializations
 */

void
init_hardware(void)
{
    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    digitalWrite(PIN_LED, LOW);
    servo.attach(PIN_SERVO);
    servo.write(0);
    delay(500);
}

void
led_setup_done( void )
{
    digitalWrite(PIN_LED, !digitalRead( PIN_LED ));
}
    
/*
 *  ----------  Subscriptions  -----------
 */

void
subscribe_to( const char *ptopic )
{
    char topic[50];

    sprintf( topic, "%s/%s/%s", main_topic, other, ptopic );
    client.subscribe( topic );
    Serial.printf( "%s: %s\n\r", __FUNCTION__, topic );
}


void
init_suscriptions( void )
{
    subscribe_to( "toggle" );
    subscribe_to( "angle" );
}

/*
 *  on subscribed actions
 */

static void
change_led( char *msg )
{
    digitalWrite(PIN_LED, !digitalRead( PIN_LED ));
}

static void
write_servo( char *msg )
{
    servo.write( atoi( msg ) );
}

//  List of suscriptions and actions

topic_t topics[] =
{
    {   "toggle", change_led  },
    {   "angle", write_servo  },
    {   NULL }
};

/*
 *  ------------    Publications    ------------
 */

void
do_publish( const char *ptopic, const char *message )
{
    char topic[50];

    sprintf( topic, "%s/%s/%s", main_topic, group, ptopic );
    client.publish( topic, message );
    Serial.printf( "%s: %s %s\n\r", __FUNCTION__, topic, message );
}


/*
 *  Obtaining data to publish
 */

//  Pushbutton

int buttonState;                    // current reading from the input pin (pushbutton)
int lastButtonState = HIGH;          // previous reading from the input pin (pushbutton)

unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 50;   // the debounce time; increase if the output flickers

static void
verify_pushbutton( void )
{
    int reading = digitalRead(PIN_BUTTON);

    // If the pushbutton state changed (due to noise or pressing it), reset the timer
    if (reading != lastButtonState)
        // Reset the debouncing timer
        lastDebounceTime = millis();
    else
    // If the pin_button state has changed, after the debounce time
    if ((millis() - lastDebounceTime) > debounceDelay)
    {
        // And if the current reading is different than the current buttonState
        if (reading != buttonState)
        {
            buttonState = reading;
            if (buttonState == LOW)
                do_publish( "toggle", "toggle" );
        }
    }
    // Save the reading. Next time through the loop, it'll be the lastButtonState
    lastButtonState = reading;
}

//  Potentiometer

int last_angle = 0;
unsigned long last_sample = 0;
const unsigned long sample_delta = 100;
const int skip_angle = 5;

static void
verify_pote( void )
{
    int sensorValue, angle;
    char buff[50];

    if( ( millis() - last_sample ) <= sample_delta )
        return;

    last_sample = millis();

    sensorValue = analogRead(ANAIN);
    angle = map( sensorValue, 0, RANGE, 0, 180 );
    angle = angle/skip_angle*skip_angle;
    if( angle != last_angle )
    {
        sprintf( buff, "%d", angle );
        do_publish( "angle", buff );
        last_angle = angle;

    }
}

/*
 *  Detecting hardware changes and 
 *  generate publications
 */

void
verify_changes( void )
{
    verify_pushbutton();
    verify_pote();
}



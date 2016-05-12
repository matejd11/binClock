#define VERBOSE

#define FIRSTLEDHOUR 2
#define FIRSTLEDMINUTE 7
#define DELTA 1000
#define DOT 13

#define BUTTON_MINUTE A0
#define BUTTON_HOUR A1
#define BUTTON_SECOUND A2

typedef struct hodiny {
    byte hour;
    byte minute;
    byte secound;
    byte dot;
    boolean btn_m;
    boolean btn_h;
    boolean btn_s;
    unsigned long ticks;
} hodiny_t;

hodiny_t cas = {0, 0, 0, 0, 0, 0, 0, 0};

void addhour(hodiny_t *data)
{
    data->hour++;
    if (data->hour >= 24) {
        data->hour = 0;
    }
}

void addminute(hodiny_t *data)
{
    data->minute++;
    if (data->minute >= 60) {
        data->minute = 0;
        addhour(data);
    }
}

void addsecound(hodiny_t *data)
{
    data->secound++;
    if (data->secound >= 60) {
        data->secound = 0;
        addminute(data);
    }
}

int tick(hodiny_t *data)
{
    byte statuscode = 0;
    unsigned long casovac = millis();
    if (data->dot == 0 && casovac > data->ticks + DELTA / 2) {
        data->dot = 1;
        statuscode = 1;
    }
    if (casovac > data->ticks + DELTA) {
        addsecound(data);
        data->ticks = casovac;
        data->dot = 0;
        statuscode = 1;
    }
    return statuscode;
}

void show(hodiny_t cas, byte firstLEDhour, byte firstLEDminute, byte dot)
{

#ifdef VERBOSE
    Serial.print(cas.hour);
    Serial.print(cas.minute);
    Serial.print(cas.secound);
    Serial.println("");
#endif

    boolean hodiny[5] = {0};
    boolean minuty[6] = {0};

    int i;

    for (i = 0; i < 5; i++) {
        hodiny[i] = (cas.hour >> i) & 0x01;
    }
    for (i = 0; i < 6; i++) {
        minuty[i] = (cas.minute >> i) & 0x01;
    }
    for (i = 4; i >= 0; i--) {
        digitalWrite(firstLEDhour + i, hodiny[i]);
    }
    for (i = 5; i >= 0; i--) {
        digitalWrite(firstLEDminute + i, minuty[i]);
    }

    digitalWrite(dot, cas.dot);
}

int getInput(hodiny_t *data, int button_minute, int button_hour, int button_secound)
{
    int status = digitalRead(button_minute);

    if (data->btn_m == 1 && status == 0) {
        data->btn_m = 0;
        addminute(data);
        return 1;
    } else if (data->btn_m == 0 && status == 1) {
        data->btn_m = 1;
    }
    status = digitalRead(button_hour);
    if (data->btn_h == 1 && status == 0) {
        data->btn_h = 0;
        addhour(data);
        return 1;
    } else if (data->btn_h == 0 && status == 1) {
        data->btn_h = 1;
    }
    status = digitalRead(button_secound);
    if (data->btn_s == 1 && status == 0) {
        data->btn_s = 0;
        data->secound = 0;
        data->ticks = millis();
        data->dot = 0;
        return 1;
    } else if (data->btn_s == 0 && status == 1) {
        data->btn_s = 1;
    }
    return 0;
}

void setup()
{
#ifdef VERBOSE
    Serial.begin(9600);
#endif

    int i;

    for (i = 0; i < 5; i++) {
        pinMode(FIRSTLEDHOUR + i, OUTPUT);
    }

    for (i = 0; i < 6; i++) {
        pinMode(FIRSTLEDMINUTE + i, OUTPUT);
    }

    pinMode(BUTTON_MINUTE, INPUT_PULLUP);
    pinMode(BUTTON_HOUR, INPUT_PULLUP);
    pinMode(BUTTON_SECOUND, INPUT_PULLUP);
}

void loop()
{
    int statuscode = 0;

    statuscode = tick(&cas);
    statuscode |= getInput(&cas, BUTTON_MINUTE, BUTTON_HOUR, BUTTON_SECOUND);

    if (statuscode == 1) {
#ifdef VERBOSE
        Serial.print(cas.hour);
        Serial.print(" ");
        Serial.print(cas.minute);
        Serial.print(" ");
        Serial.println(cas.secound);
#endif
        show(cas, FIRSTLEDHOUR, FIRSTLEDMINUTE, DOT);
    }
}

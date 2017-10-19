#include "motorcontrol.h"
#include "ui_motorcontrol.h"

MotorControl::MotorControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MotorControl)
{
    ui->setupUi(this);

    Device = new device_t;
    connect(this, NoMotorConnection,this, NoMotorConnectionProcess );
    connect(this, MotorConnectionOK,this, MotorConnectionOKProcess );

}

MotorControl::~MotorControl()
{
    delete ui;
}


void MotorControl::print_state (status_t* state)
{
    wprintf( L" rpm: %d", state->CurSpeed );
    wprintf( L" pos: %d", state->CurPosition );
    wprintf( L" upwr: %d", state->Upwr );
    wprintf( L" ipwr: %d", state->Ipwr );
    wprintf( L" flags: %x", state->Flags );
    wprintf( L" mvsts: %x", state->MvCmdSts );
    if (state->Flags & STATE_ALARM)
        wprintf( L" ALARM" );
    if (state->Flags & STATE_ERRC)
        wprintf( L" ERRC" );
    if (state->Flags & STATE_ERRD)
        wprintf( L" ERRD" );
    wprintf( L"\n" );
}

void MotorControl::string_print_state (wchar_t *string, status_t* state)
{
    swprintf( string, L" rpm: %d", state->CurSpeed );
    swprintf( string, L"%s\n pos: %d", string, state->CurPosition );
    swprintf( string, L"%s\n upwr: %d", string, state->Upwr );
    swprintf( string, L"%s\n ipwr: %d", string, state->Ipwr );
    swprintf( string, L"%s\n flags: %x", string, state->Flags );
    swprintf( string, L"%s\n mvsts: %x", string, state->MvCmdSts );
    if (state->Flags & STATE_ALARM)
        swprintf( string, L"%s\n  ALARM", string  );
    if (state->Flags & STATE_ERRC)
        swprintf( string, L"%s\n  ERRC", string );
    if (state->Flags & STATE_ERRD)
        swprintf( string, L"%s\n  ERRD", string  );
    swprintf( string, L"%s\n", string );
}

const wchar_t* MotorControl::error_string (result_t result)
{
    switch (result)
    {
        case result_error:				return L"error";
        case result_not_implemented:	return L"not implemented";
        case result_nodevice:			return L"no device";
        default:						return L"success";
    }
}



char* MotorControl::widestr_to_str (const wchar_t* str)
{
    char *result;
    mbstate_t mbs;
    size_t len;
    memset(&mbs, 0, sizeof(mbs));
    len = wcsrtombs( NULL, &str, 0, &mbs );
    if (len == (size_t)(-1))
        return NULL;
    result = (char *)malloc(sizeof(char)*(len+1));
    if (result && wcsrtombs( result, &str, len+1, &mbs ) != len)
    {
        free(result);
        return NULL;
    }
    return result;
}

const wchar_t* loglevel_string (int loglevel)
{
    switch (loglevel)
    {
        case LOGLEVEL_ERROR: 	return L"ERROR";
        case LOGLEVEL_WARNING:	return L"WARN";
        case LOGLEVEL_INFO:		return L"INFO";
        case LOGLEVEL_DEBUG:	return L"DEBUG";
        default:				return L"UNKNOWN";
    }
}

void XIMC_CALLCONV my_logging_callback(int loglevel, const wchar_t* message, void* user_data)
{
    wchar_t wbuf[2048];
    char *abuf;
    (void)user_data;
    if (loglevel > LOGLEVEL_WARNING)
        return;

    /* Print to console unicode chars */
    swprintf( wbuf, sizeof(wbuf)/sizeof(wbuf[0])-1, L"XIMC %ls: %ls", loglevel_string( loglevel ), message );
    fwprintf( stderr, L"%ls\n", wbuf );

    (void)abuf;
}


void MotorControl::InitMotorDrive()
{
    engine_settings_t engine_settings;
    engine_settings_calb_t engine_settings_calb;
    calibration_t calibration;
    device_information_t di;
    status_t state;
    result_t result;
    int names_count;
    char device_name[256];
    int i;
    const int probe_devices = 0;
    char ximc_version_str[32];
    device_enumeration_t devenum;

    //Inherit system locale
    setlocale(LC_ALL,"");
#ifdef _MSC_VER
    // UTF-16 output on windows
    _setmode( _fileno(stdout), _O_U16TEXT );
    _setmode( _fileno(stderr), _O_U16TEXT );
#endif

    set_logging_callback(my_logging_callback, NULL);
    ximc_version( ximc_version_str );

    devenum = enumerate_devices( probe_devices, NULL );
    if (!devenum)
    {
        wprintf( L"error enumerating devices\n" );
        names_count = 0;
    }

    names_count = get_device_count( devenum );
    if (names_count == -1)
    {
        wprintf( L"error enumerating device\n" );
        names_count = 0;
    }
    for (i = 0; i < names_count; ++i)
    {
        wprintf( L"device: %hs\n", get_device_name( devenum, i ) );
    }

    if (names_count == 0)
    {
        emit NoMotorConnection();
        wprintf( L"No devices found\n" );
        return;
    }
    strcpy( device_name, get_device_name( devenum, 0 ) );


    free_enumerate_devices( devenum );

    wprintf( L"\n\nOpening device...\n\n");

    *Device = open_device( device_name );

    if (*Device == device_undefined)
    {
        wprintf( L"error opening device\n" );
        return;
    }

    if ((result = get_status( *Device, &state )) != result_ok)
        wprintf( L"error getting status: %ls\n", error_string( result ) );
    print_state( &state );

    if ((result = get_device_information( *Device, &di )) != result_ok)
        wprintf( L"error getting di: %ls\n", error_string( result ) );
    wprintf( L"DI: manufacturer: %hs, id %hs, product %hs. Ver: %d.%d.%d\n",
            di.Manufacturer, di.ManufacturerId, di.ProductDescription,
            di.Major, di.Minor, di.Release);

    if ((result = command_zero( *Device )) != result_ok)
        wprintf( L"error zeroing: %ls\n", error_string( result ) );

    if ((result = get_status( *Device, &state )) != result_ok)
        wprintf( L"error getting status %ls\n", error_string( result ) );

    print_state( &state );

    if ((result = get_engine_settings( *Device, &engine_settings )) != result_ok)
        wprintf( L"error getting engine settings %ls\n", error_string( result ) );

    wprintf( L"engine: voltage %d current %d speed %d\n",
        engine_settings.NomVoltage, engine_settings.NomCurrent, engine_settings.NomSpeed );

    calibration.A = 1;
    calibration.MicrostepMode = MICROSTEP_MODE_FULL;
    if ((result = get_engine_settings_calb( *Device, &engine_settings_calb, &calibration )) != result_ok)
        wprintf( L"error getting engine calb settings %ls\n", error_string( result ) );

    wprintf( L"engine calb: voltage %d current %d speed %f\n",
        engine_settings_calb.NomVoltage, engine_settings_calb.NomCurrent, engine_settings_calb.NomSpeed );


    for (int i = 0; i < 100000; i++);
        i++;

    command_zero( *Device );

    emit MotorConnectionOK();
    return;
}

void MotorControl::NoMotorConnectionProcess()
{
    ui->checkBox->setChecked(0);
}
void MotorControl::MotorConnectionOKProcess()
{
    ui->checkBox->setChecked(1);
}
void MotorControl::on_btnInitDrive_clicked()
{
    InitMotorDrive();
}

void MotorControl::on_btnHomeDrive_clicked()
{
    command_home( *Device );
}

void MotorControl::on_btnZeroDrive_clicked()
{
    command_zero( *Device );
}

void MotorControl::on_StartFixedPosMove_clicked()
{
    command_zero( *Device );
    int angle = ui->doubleSpinBox->value()*100;
    command_move( *Device, angle, 0 );
}

void MotorControl::on_btnRight_clicked()
{
    command_right( *Device );
}

void MotorControl::on_btnLeft_clicked()
{
    command_left( *Device );
}

void MotorControl::on_btnStopMotor_clicked()
{
    command_stop( *Device );
}

void MotorControl::on_btnGetStatus_clicked()
{
    if (Device)
    {
        status_t state;
        int result = get_status( *Device, &state);
        if (result == result_ok)
            ui->textBrowser->append("Status");
        else
            ui->textBrowser->append("Get Status Error");
    }
    else
        ui->textBrowser->append("No Device connected");
}

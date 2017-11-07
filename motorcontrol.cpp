#include "motorcontrol.h"


void Ctrl_loop::StartLoop(void)
{
    while(stopThreadFlag == 0)
    {
        if (*Device)
        {
            int result = get_status( *Device, State);
            if (result == result_ok)
                emit SendStatus(State);
            else
                emit ConnectionError();
            if (State->CurSpeed == 0 && Ready == false)
            {
                Ready = true;
                emit MoveDone();
            }

        }
        QThread::msleep(200);
    }
}
Ctrl_loop::Ctrl_loop( device_t * D )
{
    State = new status_t;
    Device = D;
    Ready = true;
    hThread = new QThread(this);
    stopThreadFlag = 0;
    connect(hThread, QThread::started, this, Ctrl_loop::StartLoop);

    this->moveToThread(hThread);

}
Ctrl_loop::~Ctrl_loop()
{
    delete(hThread);
    delete(State);
}
void Ctrl_loop::GetAngleFromCam(float Angle)
{
    command_zero( *Device );
    int IntAngle = (int)(Angle*100);
    command_move( *Device, IntAngle, 0 );
    Ready = false;
}

void Ctrl_loop::stopThread()
{
    stopThreadFlag = 1;
    hThread->wait();
    delete(hThread);
}
void Ctrl_loop::startThread()
{
    stopThreadFlag = 0;
    hThread->start();
}
MotorControl::MotorControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MotorControl),
    LogBox(new Ui::Dialog)
{
    ui->setupUi(this);

    Device = new device_t;
    *Device = 0;
    connect(this, NoMotorConnection,this, NoMotorConnectionProcess );
    connect(this, MotorConnectionOK,this, MotorConnectionOKProcess );

    ControlLoop = new Ctrl_loop( Device );

    connect(ControlLoop, Ctrl_loop::SendStatus, this, UpdateStatus );
    connect(this, MotorConnectionOK,this, MotorConnectionOKProcess );
    connect(ControlLoop, Ctrl_loop::ConnectionError, this, NoMotorConnection );
    connect(ControlLoop, Ctrl_loop::MoveDone, this, GetMoveDone );
    connect(this, SendAngleFromCam, ControlLoop, Ctrl_loop::GetAngleFromCam);
    ControlLoop->startThread();

    //Disable Internal log browser of Motor drive class
    ui->btnOpenLog->setVisible(false);
    LogDialogBox = new QDialog;
    LogBox->setupUi(LogDialogBox);
    connect( LogBox->btnExit, QPushButton::pressed, this, CloseLogDialog);
    connect( this, AppendTextToLog, LogBox->LogTextWindow, QTextBrowser::append);

}
MotorControl::~MotorControl()
{
    ControlLoop->stopThread();
    command_stop( *Device );
    delete(Device);
    delete(ControlLoop);
    delete(LogDialogBox);
    delete ui;
    delete LogBox;
}
void MotorControl::GetAngleFromCam( float Angle)
{
     ControlLoop->GetAngleFromCam(Angle);
}
void MotorControl::GetMoveDone()
{
     emit SendMoveDone();
}

void MotorControl::CloseLogDialog()
{
    LogDialogBox->hide();
}
void MotorControl::UpdateStatus(status_t * Status)
{
    wprintf( L" rpm: %d", Status->CurSpeed );
    wprintf( L" pos: %d", Status->CurPosition );
    wprintf( L" upwr: %d", Status->Upwr );
    wprintf( L" ipwr: %d", Status->Ipwr );
    wprintf( L" flags: %x", Status->Flags );
    wprintf( L" mvsts: %x", Status->MvCmdSts );
    if (Status->Flags & STATE_ALARM)
        wprintf( L" ALARM" );
    if (Status->Flags & STATE_ERRC)
        wprintf( L" ERRC" );
    if (Status->Flags & STATE_ERRD)
        wprintf( L" ERRD" );
    wprintf( L"\n" );

    ui->textBrowser->clear();
    ui->textBrowser->append("rpm: " + QString::number( Status->CurSpeed));
    ui->textBrowser->append("pos: " + QString::number( Status->CurPosition));
    ui->textBrowser->append("upwr: " + QString::number( Status->Upwr));
    ui->textBrowser->append("ipwr: " + QString::number( Status->Ipwr));
    ui->textBrowser->append("flags: " + QString::number( Status->Flags));
    ui->textBrowser->append("mvsts: " + QString::number( Status->MvCmdSts));

    if (Status->Flags & STATE_ALARM)
        ui->textBrowser->append("ALARM!!!");
    if (Status->Flags & STATE_ERRC)
        ui->textBrowser->append("ERRC");
    if (Status->Flags & STATE_ERRD)
        ui->textBrowser->append("ERRD");

    emit AppendTextToLog("Status update DONE" );
}
void MotorControl::printStateToStr (QString string, status_t * state)
{
    string = " rpm: " + QString::number( state->CurSpeed );
    string = string + "\n pos: " + QString::number( state->CurPosition );
    string = string + "\n upwr: " + QString::number( state->Upwr );
    string = string + "\n ipwr: " + QString::number( state->Ipwr );
    string = string + "\n flags: " + QString::number( state->Flags );
    string = string + "\n mvsts: " + QString::number( state->MvCmdSts );
    if (state->Flags & STATE_ALARM)
         string = string + "\nALARM!!!";
    if (state->Flags & STATE_ERRC)
        string = string + "\nERRC";
    if (state->Flags & STATE_ERRD)
        string = string + "\nERRD\n";
}
void XIMC_CALLCONV my_logging_callback(int loglevel, const wchar_t* message, void* user_data)
{
    wchar_t wbuf[2048];
    char *abuf;
    (void)user_data;
    if (loglevel > LOGLEVEL_WARNING)
        return;

    switch (loglevel)
    {
        case LOGLEVEL_ERROR:
                swprintf( wbuf, sizeof(wbuf)/sizeof(wbuf[0])-1, L"XIMC ERROR: %ls", L"ERROR", message );
                break;
        case LOGLEVEL_WARNING:
                swprintf( wbuf, sizeof(wbuf)/sizeof(wbuf[0])-1, L"XIMC WARN: %ls", message );
                break;
        case LOGLEVEL_INFO:
                swprintf( wbuf, sizeof(wbuf)/sizeof(wbuf[0])-1, L"XIMC INFO: %ls", message );
                break;
        case LOGLEVEL_DEBUG:
                swprintf( wbuf, sizeof(wbuf)/sizeof(wbuf[0])-1, L"XIMC DEBUG: %ls", message );
                break;
        default:
                swprintf( wbuf, sizeof(wbuf)/sizeof(wbuf[0])-1, L"XIMC UNKNOWN: %ls", message );
                break;
    }
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



    emit AppendTextToLog("Starting motor init procedure", "MOTOR" );
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
        emit AppendTextToLog("Error enumerating devices. Motor init failed.", "ERROR" );
        names_count = 0;
    }

    names_count = get_device_count( devenum );
    if (names_count == -1)
    {
        wprintf( L"error enumerating device\n" );
        emit AppendTextToLog("Error enumerating devices. Motor init failed.", "ERROR" );
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

    emit AppendTextToLog("Opening device", "MOTOR" );
    wprintf( L"\n\nOpening device...\n\n");

    if (Device != 0) close_device( Device );

    *Device = open_device( device_name );

    if (*Device == device_undefined)
    {
        Device = 0;
        wprintf( L"error opening device\n" );
        emit AppendTextToLog("Error opening device. Motor init failed.", "ERROR" );
        emit NoMotorConnection();
        return;
    }

    if ((result = get_status( *Device, &state )) != result_ok)
    {
        wprintf( L"error getting status: %ls\n", error_string( result ) );
        emit AppendTextToLog("Error getting status", "WARNG" );
    }
    UpdateStatus( &state );

    if ((result = get_device_information( *Device, &di )) != result_ok)
    {
        wprintf( L"error getting di: %ls\n", error_string( result ) );
        emit AppendTextToLog("Error getting device info.", "WARNG" );
    }
    wprintf( L"DI: manufacturer: %hs, id %hs, product %hs. Ver: %d.%d.%d\n",
            di.Manufacturer, di.ManufacturerId, di.ProductDescription,
            di.Major, di.Minor, di.Release);
    wchar_t tmp[255];
    swprintf( tmp, L"DI: manufacturer: %hs, id %hs, product %hs. Ver: %d.%d.%d\n",
            di.Manufacturer, di.ManufacturerId, di.ProductDescription,
            di.Major, di.Minor, di.Release);
    QString tmpStr;
    tmpStr.fromWCharArray(tmp);
    emit AppendTextToLog(tmpStr, "MOTOR" );
    //delete(tmpStr);
    if ((result = command_zero( *Device )) != result_ok)
    {
        wprintf( L"error zeroing: %ls\n", error_string( result ) );
        emit AppendTextToLog("error zeroing motor", "WARNG" );
    }
    if ((result = get_status( *Device, &state )) != result_ok)
    {
        wprintf( L"error getting status %ls\n", error_string( result ) );
        emit AppendTextToLog("Error getting status", "WARNG" );

    }
    UpdateStatus( &state );

    if ((result = get_engine_settings( *Device, &engine_settings )) != result_ok)
    {
        wprintf( L"error getting engine settings %ls\n", error_string( result ) );       
        emit AppendTextToLog("Error getting engine settings", "WARNG" );
    }
    wprintf( L"engine: voltage %d current %d speed %d\n",
        engine_settings.NomVoltage, engine_settings.NomCurrent, engine_settings.NomSpeed );
    swprintf( tmp, L"engine: voltage %d current %d speed %d\n",
        engine_settings.NomVoltage, engine_settings.NomCurrent, engine_settings.NomSpeed );
    tmpStr.clear();
    tmpStr.fromWCharArray(tmp);
    emit AppendTextToLog(tmpStr, "MOTOR" );

    calibration.A = 1;
    calibration.MicrostepMode = MICROSTEP_MODE_FULL;
    if ((result = get_engine_settings_calb( *Device, &engine_settings_calb, &calibration )) != result_ok)
    {
        wprintf( L"error getting engine calb settings %ls\n", error_string( result ) );
        emit AppendTextToLog("Error getting engine calb settings", "WARNG" );
    }
    wprintf( L"engine calb: voltage %d current %d speed %f\n",
        engine_settings_calb.NomVoltage, engine_settings_calb.NomCurrent, engine_settings_calb.NomSpeed );
    swprintf( tmp, L"engine calb: voltage %d current %d speed %f\n",
        engine_settings_calb.NomVoltage, engine_settings_calb.NomCurrent, engine_settings_calb.NomSpeed );
    tmpStr.clear();
    tmpStr.fromWCharArray(tmp);
    emit AppendTextToLog(tmpStr, "MOTOR" );

    if ((result = command_zero( *Device )) != result_ok)
    {
        wprintf( L"error zeroing: %ls\n", error_string( result ) );
        emit AppendTextToLog("error zeroing motor", "WARNG" );
    }
    emit AppendTextToLog("Motor drive init DONE", "MOTOR" );
    emit MotorConnectionOK();
    return;
}
void MotorControl::NoMotorConnectionProcess()
{
    emit AppendTextToLog("Motor drive is not connected", "WARNG" );

    ui->textBrowser->clear();
    ui->textBrowser->append("No motor drive connected");
    ui->checkBox->setChecked(0);

    ui->WidgetControlButtons->setEnabled(false);

}
void MotorControl::MotorConnectionOKProcess()
{
    ui->checkBox->setChecked(1);
    ui->WidgetControlButtons->setEnabled(true);
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
void MotorControl::on_btnOpenLog_clicked()
{
    LogDialogBox->show();
}

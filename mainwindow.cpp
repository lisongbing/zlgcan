#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QString>
#include "zlgcan.h"
#include "canreceive.h"
#include "cansend.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QObject::connect(ui->btncon,SIGNAL(clicked()),this,SLOT(on_btncon_click()));
    QObject::connect(ui->lamba,&QPushButton::clicked,[=]{
        qDebug() << "这是第lamba按钮";
    });
    QObject::connect(ui->point,&QPushButton::clicked,this,&MainWindow::point_click);

    QObject::connect(ui->openButton,SIGNAL(clicked()),this,SLOT(on_openbtn_clicked()));

    //QObject::connect(ui->closeButton,SIGNAL(clicked()),this,SLOT(on_closeButton_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    qDebug() << "这是第一个按钮";
}

void MainWindow::on_btncon_click()
{
    qDebug() << "这是第click按钮";
}

void MainWindow::point_click()
{
    qDebug() << "点击,解除限位保护";
    canSender cSender;
    uint8_t data[8] = {0xF6,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}

void MainWindow::on_openbtn_clicked()
{
    int deviceType = this->ui->comboBox->currentIndex();
    QString deviceIndex = this->ui->lineEdit->text();
    int deviceChannel = this->ui->comboBox_2->currentIndex();
    qDebug() << deviceType;
    qDebug() << deviceIndex;
    qDebug() << deviceChannel;

    //ZCAN_USBCAN_2E_U 为设备类型, 请根据实际修改
    this->canHandle = ZCAN_OpenDevice(ZCAN_USBCAN_E_U, 0, 0);
    if (INVALID_DEVICE_HANDLE == this->canHandle)
    {
        qDebug()  << "打开设备失败";
        return;
    }
    //CAN 设备设置波特率的 key 为 baud_rate，值 1000000 为 1000kbps, 800000 为 800kbps, 其它请查看
    //属性表
    //若为 CANFD 设备, 设置冲裁域波特率的 key 为 canfd_abit_baud_rate，数据域波特率为
    //canfd_dbit_baud_rate，请注意区分 CAN 和 CANFD 设备设置波特率的区别。
    if (ZCAN_SetValue (this->canHandle , "0/baud_rate", "250000") != STATUS_OK)
    {
        qDebug()  << "设置波特率失败";

    }
    else
    {
        ZCAN_CHANNEL_INIT_CONFIG cfg;
        memset(&cfg, 0, sizeof(cfg));
        cfg.can_type = TYPE_CAN;//CANFD 设备为 TYPE_CANFD
        cfg.can.filter = 0;
        cfg.can.mode = 0; //正常模式, 1 为只听模式
        cfg.can.acc_code = 0;
        cfg.can.acc_mask = 0xffffffff;
        CHANNEL_HANDLE chHandle = ZCAN_InitCAN(this->canHandle, 0, &cfg);
        if (INVALID_CHANNEL_HANDLE == chHandle)
        {
            qDebug()  << "初始化通道失败" ;
        }
        else if (ZCAN_StartCAN(chHandle) != STATUS_OK)
        {
            qDebug()  << "启动通道失败";

        }
        else
        {

            //创建一个接收线程
            //创建子线程
            CANReceive * subThread = new CANReceive;
            subThread->setHandle(chHandle);
            // 启动子线程
            subThread->start();

            ZCAN_Transmit_Data frame;
            memset(&frame, 0, sizeof(frame));
            frame.frame.can_id = MAKE_CAN_ID(0x100, 1, 0, 0);
            frame.frame.can_dlc = 8;
            frame.transmit_type = 0;
            BYTE data[] = {1, 2, 3, 4, 5, 6, 7, 8};
            memcpy(frame.frame.data, data, sizeof(data));
            if (ZCAN_Transmit(this->canHandle, &frame, 1) != 1)
            {
                qDebug()  << "发送数据失败";
            }
            ui->openButton->setText(QString("已打开"));
        }
    }
    //ZCAN_CloseDevice(dhandle);
}

void MainWindow::on_closeButton_clicked()
{
    qDebug() << "点击close 按钮";
    //判断是否已经打开
    if (this->canHandle == NULL)
    {
        qDebug() << "设备没有打开";
        //todo 弹出提示框 设备没有打开
    }
    else{
        qDebug() << "设备已打开,正在关闭";
         ZCAN_CloseDevice(this->canHandle);
         this->canHandle = NULL;
         ui->openButton->setText(QString("打开"));
          qDebug() << "设备已关闭";
    }
}
//上升
void MainWindow::on_pushButton_2_clicked()
{
    qDebug() << "点击,上升";
    canSender cSender;
    uint8_t data[8] = {0xF1,0x01,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}
//下降
void MainWindow::on_pushButton_3_clicked()
{
    qDebug() << "点击,下降";
    canSender cSender;
    uint8_t data[8] = {0xF1,0x02,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}

//举升停止
void MainWindow::on_pushButton_4_clicked()
{
    qDebug() << "点击,举升停止";
    canSender cSender;
    uint8_t data[8] = {0xF1,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}

//方位 左
void MainWindow::on_pushButton_5_clicked()
{
    qDebug() << "点击,方位 左";
    canSender cSender;
    uint8_t data[8] = {0xF2,0x01,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}
//方位 右
void MainWindow::on_pushButton_6_clicked()
{
    qDebug() << "点击,方位 右";
    canSender cSender;
    uint8_t data[8] = {0xF2,0x02,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}
// 方位停止
void MainWindow::on_pushButton_7_clicked()
{
    qDebug() << "点击,方位停止";
    canSender cSender;
    uint8_t data[8] = {0xF2,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}

//方位 使能
void MainWindow::on_pushButton_21_clicked()
{
    qDebug() << "点击,方位使能";
    canSender cSender;
    uint8_t data[8] = {0xF2,0x03,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}

//方位 禁能
void MainWindow::on_pushButton_22_clicked()
{
    qDebug() << "点击,方位禁能";
    canSender cSender;
    uint8_t data[8] = {0xF2,0x04,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}

//俯仰 上
void MainWindow::on_pushButton_8_clicked()
{
    qDebug() << "点击,俯仰 上";
    canSender cSender;
    uint8_t data[8] = {0xF3,0x01,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}
//俯仰 下
void MainWindow::on_pushButton_9_clicked()
{
    qDebug() << "点击,俯仰 下";
    canSender cSender;
    uint8_t data[8] = {0xF3,0x02,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}
//俯仰 停止
void MainWindow::on_pushButton_10_clicked()
{
    qDebug() << "点击,俯仰 停止";
    canSender cSender;
    uint8_t data[8] = {0xF3,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}

//俯仰 使能
void MainWindow::on_pushButton_23_clicked()
{
    qDebug() << "点击,俯仰使能";
    canSender cSender;
    uint8_t data[8] = {0xF3,0x03,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}
//俯仰 禁能
void MainWindow::on_pushButton_24_clicked()
{
    qDebug() << "点击,俯仰禁能";
    canSender cSender;
    uint8_t data[8] = {0xF3,0x04,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}

//伸缩 伸出
void MainWindow::on_pushButton_11_clicked()
{
    qDebug() << "点击,伸缩 伸出";
    canSender cSender;
    uint8_t data[8] = {0xF4,0x01,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}
//伸缩 缩
void MainWindow::on_pushButton_12_clicked()
{
    qDebug() << "点击,伸缩 缩";
    canSender cSender;
    uint8_t data[8] = {0xF4,0x02,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}
//伸缩 停
void MainWindow::on_pushButton_13_clicked()
{
    qDebug() << "点击,伸缩 停";
    canSender cSender;
    uint8_t data[8] = {0xF4,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}
//夹住
void MainWindow::on_pushButton_14_clicked()
{
     qDebug() << "点击,夹住";
     canSender cSender;
     uint8_t data[8] = {0xF5,0x01,0x00,0x00,0x00,0x00,0x00,0x00};
     cSender.Send(this->canHandle,0x112,data,8);
}
//松开
void MainWindow::on_pushButton_15_clicked()
{
    qDebug() << "点击,松开";
    canSender cSender;
    uint8_t data[8] = {0xF5,0x02,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}
//抓手 停止
void MainWindow::on_pushButton_16_clicked()
{
    qDebug() << "点击,抓手 停止";
    canSender cSender;
    uint8_t data[8] = {0xF5,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}
//开始 激震
void MainWindow::on_pushButton_17_clicked()
{
    int direction = this->ui->jizhen_direction->currentIndex();
    int speed = this->ui->lineEdit_2->text().toInt();
    int freq = this->ui->lineEdit_3->text().toInt();

    qDebug() << "点击,开始 激震" << direction << speed << freq;
    if (speed < 0)
    {
        speed = -speed;
    }

    if (freq < 0)
    {
        freq = -freq;
    }


    canSender cSender;
    uint8_t data[8] = {0xF7,0x01,(uint8_t)(speed>>8),(uint8_t)speed,(uint8_t)(freq>>8),(uint8_t)freq,(uint8_t)direction,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}
//开始 激震 收线
void MainWindow::on_pushButton_25_clicked()
{
    qDebug() << "点击,开始 激震 收线";
    canSender cSender;
    uint8_t data[8] = {0xF7,0x02,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}
//开始 激震 放线
void MainWindow::on_pushButton_26_clicked()
{
    qDebug() << "点击,开始 激震 放线";
    canSender cSender;
    uint8_t data[8] = {0xF7,0x03,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}

//停止 激震
void MainWindow::on_pushButton_18_clicked()
{
    qDebug() << "点击,停止 激震";
    canSender cSender;
    uint8_t data[8] = {0xF7,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}

//48V上电
void MainWindow::on_pushButton_19_clicked()
{
    qDebug() << "点击,上电";
    canSender cSender;
    uint8_t data[8] = {0xF8,0x01,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}
//48V 下电
void MainWindow::on_pushButton_20_clicked()
{
    qDebug() << "点击,下电";
    canSender cSender;
    uint8_t data[8] = {0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}




//设置偏置参数
void MainWindow::on_pushButton_27_clicked()
{
    int torque = this->ui->lineEdit_4->text().toInt();
    int offset = this->ui->lineEdit_5->text().toInt();
    //int freq = this->ui->lineEdit_3->text().toInt();

    qDebug() << "设置偏置参数";
    qDebug() << torque << offset;

    canSender cSender;
    uint8_t data[8] = {0xF7,0x04,(uint8_t)offset,(uint8_t)(torque>>8),(uint8_t)torque,0x00,0x00,0x00};
    cSender.Send(this->canHandle,0x112,data,8);
}

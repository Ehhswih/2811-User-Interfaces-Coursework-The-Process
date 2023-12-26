#include "playsetting.h"
#include "ui_playsetting.h"

playSetting::playSetting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::playSetting)
{
    ui->setupUi(this);
    ui->brightnessSlider->setRange(-100, 100);
    ui->contrastslider->setRange(-100, 100);
    ui->hueslider->setRange(-100, 100);
    ui->saturationslider->setRange(-100, 100);
}

playSetting::~playSetting()
{
    delete ui;
}

void playSetting::setBrightnessValue(int value)
{
    ui->brightnessSlider->setValue(value);
}

void playSetting::setContrastValue(int value)
{
    ui->contrastslider->setValue(value);
}

void playSetting::setHueValue(int value)
{
    ui->hueslider->setValue(value);
}

void playSetting::setSaturationValue(int value)
{
    ui->saturationslider->setValue(value);
}

void playSetting::on_comboBox_currentIndexChanged(int index)
{
    emit comboBox_indexChanded(index);
}


void playSetting::on_brightnessSlider_sliderMoved(int position)
{
    emit brightness_moved(position);
}


void playSetting::on_contrastslider_sliderMoved(int position)
{
    emit contrast_moved(position);
}


void playSetting::on_hueslider_sliderMoved(int position)
{
    emit hue_moved(position);
}


void playSetting::on_saturationslider_sliderMoved(int position)
{
    emit saturation_moved(position);
}


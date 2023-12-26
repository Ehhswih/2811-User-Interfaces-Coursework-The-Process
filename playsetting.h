#ifndef PLAYSETTING_H
#define PLAYSETTING_H

#include <QWidget>

namespace Ui {
class playSetting;
}

class playSetting : public QWidget
{
    Q_OBJECT

public:
    explicit playSetting(QWidget *parent = nullptr);
    ~playSetting();


public slots:

    void on_comboBox_currentIndexChanged(int index);

    void on_brightnessSlider_sliderMoved(int position);

    void on_contrastslider_sliderMoved(int position);

    void on_hueslider_sliderMoved(int position);

    void on_saturationslider_sliderMoved(int position);

    void setBrightnessValue(int value);
    void setContrastValue(int value);
    void setHueValue(int value);
    void setSaturationValue(int value);

signals:
    void btn_cut_clicked();
    void comboBox_indexChanded(int index);
    void brightness_moved(int position);
    void contrast_moved(int position);
    void hue_moved(int position);
    void saturation_moved(int position);

private:
    Ui::playSetting *ui;
};

#endif // PLAYSETTING_H

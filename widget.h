#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QListWidget>
#include "myview.h"
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    myView *view;

    StickFigure* addStick();
    void addFrame();
    void copyFrame(QDataStream &stream, Frame *destination);
    Frame *setUpFrame(int pos);
    void createPaths();
    void detectLibraries();
private slots:
    void on_drawBtn_clicked();

    void on_cursor_clicked();

    void on_addStickBtn_clicked();

    void on_thicknessSpinBox_valueChanged(int arg1);

    void on_stickLayerView_itemClicked(QListWidgetItem *item);

    void on_stickLayerView_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_deleteStickFigureBtn_clicked();

    void on_moveLayerUpBtn_clicked();

    void on_moveLayerDownBtn_clicked();

    void on_drawCircleBtn_clicked();

    void on_chooseColorBtn_clicked();

    void on_undoBtn_clicked();

    void on_redoBtn_clicked();

    void on_addFrameBtn_clicked();

    void on_frameListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_copyFrame_clicked();

    void on_onionSkinSpinBox_valueChanged(const QString &arg1);

    void on_deleteFrameBtn_clicked();

    void on_frameListWidget_itemClicked(QListWidgetItem *item);

    void on_frameListWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_stickLayerView_itemDoubleClicked(QListWidgetItem *item);

    void on_PlayButton_clicked();

    void on_zoomInBtn_clicked();

    void on_zoomOutBtn_clicked();

    void on_resetZoomBtn_clicked();

    void on_exportBtn_clicked();

    void on_drawImageBtn_clicked();

    void on_imgHOffsetSlider_sliderMoved(int position);

    void on_imgWSlider_sliderMoved(int position);

    void on_imgHSlider_sliderMoved(int position);

    void on_imgVOfsetSlider_sliderMoved(int position);

    void on_imgRotationSlider_sliderMoved(int position);

    void on_imgHOffsetSlider_sliderReleased();

    void on_imgVOfsetSlider_sliderReleased();

    void on_imgWSlider_sliderReleased();

    void on_imgHSlider_sliderReleased();

    void on_imgRotationSlider_sliderReleased();

    void on_saveLibraryBtn_clicked();

    void on_addItemToLibBtn_clicked();

    void on_loadLibraryBtn_clicked();

    void on_refreshLibrariesBtn_clicked();

    void on_libraryList_currentRowChanged(int currentRow);


    void on_removeFromLibraryBtn_clicked();

    void on_addItemFromLibraryToSceneBtn_clicked();

    void on_saveCurrentLibraryBtn_clicked();

    void on_imgHOffsetSlider_valueChanged(int value);

    void on_imgVOfsetSlider_valueChanged(int value);

    void on_imgWSlider_valueChanged(int value);

    void on_imgHSlider_valueChanged(int value);

    void on_imgRotationSlider_valueChanged(int value);

    void on_imgHOffsetSpinbox_valueChanged(int arg1);

    void on_imgVOffsetSpinbox_valueChanged(int arg1);

    void on_imgWidthSpinbox_valueChanged(int arg1);

    void on_imgHeightSpinbox_valueChanged(int arg1);

    void on_imgRotationSpinbox_valueChanged(int arg1);

    void on_setStickColorBtn_clicked();

    void on_setAllSticksColorBtn_clicked();

    void on_setPenThicknessBtn_clicked();

    void on_setPenThicknessAllBtn_clicked();

    void on_selectBrushColorBtn_clicked();

    void on_setAllBrushColorBtn_clicked();

    void on_setBrushColorBtn_clicked();

    void on_manipulateBtn_clicked();

    void on_penOpacitySlider_valueChanged(int value);

    void on_fillOpacitySlider_valueChanged(int value);

    void on_penOpacitySpinbox_valueChanged(int arg1);

    void on_fillOpacitySpinbox_valueChanged(int arg1);

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H

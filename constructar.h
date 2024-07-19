#ifndef CONSTRUCTAR_H
#define CONSTRUCTAR_H

#include <QMainWindow>
#include <QThread>
#include <QKeyEvent>
#include "Camera.h"
#include "themanual.h"
#include <iostream>
#include "QMessageBox"
#include <QFileDialog>
#include <QPixmap>
#include <QTimer>
#include <QDesktopServices>


//Header file for the ConstructAR class, which represents the main window in the application

//Add the class to the Ui namespace
namespace Ui {
class ConstructAR;
}

//Class definition
class ConstructAR : public QMainWindow //inherits QMainWindow class
{
    Q_OBJECT //Needed to indicate that this class is a Q object, this is required for Qt specific features like signals and slots to work

public:
    explicit ConstructAR(QWidget *parent = nullptr); //Class constructor
    ~ConstructAR(); //Class destructor

    void init();   //Function that initializes the ConstructAR object, when created

    //Signals are functions which emit events, used by slots (functions that capture events).
    //This is how objects running on different threads can communicate to each other and pass data along, without freezing the program (UI)
signals:
    //Signal emitted when when a user either starts creating or recreating a mosaic
    void startTheManual(std::string path = "LegoModel.xml", std::string modelPicturePath = "LegoModel.jpg", bool isCreating = true);

private slots:
    void on_newFile_clicked();

    void on_openFile_clicked();

    //Slot for updating the camera frame in the program window
    void updateFrameLabel(cv::Mat frame);

    //Slot which updates the picture of the block that the user is expected to place next
    void updateExpectedBlockLabel(QString path, QString blockType, QString blockColor);

    //Slot which updates the picture of the block that the user actually placed or removed
    void updateDetectedBlockLabel(QString _path, bool _isBlockRemoved);
    void refreshDetectedBlockLabel();

    void on_saveFile_clicked();

    void on_reset_clicked();

    void on_shareFile_clicked();

    void on_exit_clicked();

    //Slot which updates the content of the infoLabel based on whether a block is placed or removed and on whether it is correct or not
    //the infoLabel displays text guidance to the user
    void blockDetected(bool isCorrect, bool isRemoved, bool isCreating);

    void on_mosaic_loaded(int maximum);

    //Slot which updates the progress bar and the current step while the user advances in recreating a mosaic
    void on_step_changed(int step);

    //Slot used for displaying the description of a mosaic (A list of used blocks, showing number of blocks required from each type and color)
    void on_description_done();

private:
    Ui::ConstructAR *ui;        //Pointer to the ui object which contains pointers to each UI widget in the window
    QThread* creatorThread;     //Pointer to the thread on which the actual lego manual runs (block detection etc)

    TheManual* theManual;       //Pointer to the legoManual object which processes everything
    void keyPressEvent(QKeyEvent *);        //A function for handling keypress events - used for debugging
    QString mosaicName;                     //The name of the mosaic being (re)created
    QString dbPath;
    bool isBlockRemoved;
    QImage loadingBG, initialBG, initialDB, loadedMosaic;   //Images containing the initial and loading screens as well as the image of the final result


    //A geometry is a QRect object which holds (x, y) coordinates of top left corner and (width, height) lengths
    //It is used for placing the widget in the desired position and for giving it the desired size
    //LABEL GEOMETRY FOR CREATE STATE
    const QRect createMdbLabelGeometry = QRect(1060, 210, 300, 30);
    const QRect createMdetectedBlockLabelGeometry = QRect(1050, 240, 320, 240);

    const QRect createMdetailsLabelGeometry = QRect(1420, 210, 300, 30);
    const QRect createMdetailsFrameGeometry = QRect(1410, 240, 400, 500);
    const QRect createMblocksUsedNeededGeometry = QRect(1440, 250, 300, 30);
    const QRect createMblockListGeometry = QRect(1445, 280, 341, 391);
    const QRect createMtotalNumberStepsLabelGeometry = QRect(1440, 675, 300, 30);
    const QRect createMtotalNumberStepsGeometry = QRect(1470, 700, 300, 30);

    //LABEL GEOMETRY FOR RECREATE STATE
    const QRect recreateMdetailsLabelGeometry = QRect(1420, 110, 300, 30);
    const QRect recreateMdetailsFrameGeometry = QRect(1410, 140, 400, 500);
    const QRect recreateMblocksUsedNeededGeometry = QRect(1440, 150, 300, 30);
    const QRect recreateMblockListGeometry = QRect(1445, 180, 341, 391);
    const QRect recreateMtotalNumberStepsLabelGeometry = QRect(1440, 575, 300, 30);
    const QRect recreateMtotalNumberStepsGeometry = QRect(1470, 600, 300, 30);

    const QRect recreateMdbLabelGeometry = QRect(1060, 430, 300, 30);
    const QRect recreateMdetectedBlockLabelGeometry = QRect(1050, 460, 320, 240);

    void showCreateUI();        //A function which updates the UI with the required widgets for creating a lego mosaic
    void showRecreateUI();      //Equivalent function for recreating a mosaic

    void toggleMosaicDescription(bool show, bool isCreating = false);  //Either show or hide the details list of blocks for a mosaic
};

#endif // CONSTRUCTAR_H

#include "constructar.h"
#include "ui_constructar.h"

//Class constructor
ConstructAR::ConstructAR(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConstructAR)
{
    //Setting the UI pointer, registering meta types in order to be able to pass them as arguments in signals and slots
    ui->setupUi(this);
    qRegisterMetaType< cv::Mat >("cv::Mat");
    qRegisterMetaType< std::string >("std::string");
    qRegisterMetaType< bool >("bool");

    //Calling the init() function
    init();

}

ConstructAR::~ConstructAR()
{
    delete ui;
}

//INIT FUNCTION
void ConstructAR::init(){
    //Pixmap object holding the picture of the program background
    QPixmap bkgnd(":/Icons/background.jpg");

    //QPalette objects used for setting different color properties of different widgets
    QPalette mainWindowPalette, projectTitlePalette, infoLabelPalette;

    //Loading the specified images as QImages
    initialBG = QImage(":/Icons/initialBG.jpg");
    loadingBG = QImage(":/Icons/loadingBG.jpg");
    initialDB = QImage(":/Icons/initialDB.jpg");

    //Set the background of the main window to the previously loaded image
    mainWindowPalette.setBrush(QPalette::Window, bkgnd);

    //Update the palettes of the main window to apply changes
    this->setPalette(mainWindowPalette);

    //Show window in full screen
    QMainWindow::showMaximized();

    //Modify the color of the QPalette properties of the specified palettes
    //Window - holds the background color
    //WindowText - holds the color of the text written in the widget
    projectTitlePalette.setColor(QPalette::Window, QColor(Qt::white));
    infoLabelPalette.setColor(QPalette::Window, QColor(Qt::gray));
    infoLabelPalette.setColor(QPalette::WindowText, QColor(Qt::white));

    //Update the specified palettes and set welcoming text and tip in the specified widgets
    ui->projectTitle->setPalette(projectTitlePalette);
    ui->infoLabel->setPalette(infoLabelPalette);
    ui->projectTitle->setText("  Welcome to ConstructAR!");
    ui->infoLabel->setText("  Tip: You can choose to either build a mosaic by clicking on New file, or rebuild a mosaic by clicking on Open file.");

    //Set the picture of the frame label to the initial image
    ui->frameLabel->setPixmap(QPixmap::fromImage(initialBG));

    //Hide extra widgets, not needed in the welcoming screen and disable share and save files, because no mosaic is built yet
    ui->dbLabel->hide();
    ui->ebLabel->hide();
    ui->frLabel->hide();
    ui->finalResultLabel->hide();
    ui->expectedBlockLabel->hide();
    ui->detectedBlockLabel->hide();
    ui->progressBar->hide();
    ui->progressLabel->hide();
    ui->shareFile->setDisabled(true);
    ui->saveFile->setDisabled(true);
    ui->reset->setDisabled(true);
    //Hide the mosaic description widgets
    toggleMosaicDescription(false);

    //update the creator thread pointer to point to a newly defined QThread object
    creatorThread = new QThread;

    //Update theManual pointer
    theManual = new TheManual();
    //Set theManual object to be run by creatorThread
    theManual->moveToThread(creatorThread);

    //Connect all the signals and slots
    //First argument - the object the signal belongs to
    //Second argument - the signal itself
    //Third argument - the object, the slot belongs to
    //fourth argument - the slot itself
    connect(this, SIGNAL(startTheManual(std::string, std::string, bool)), theManual, SLOT(startTheManual(std::string, std::string, bool)));

    connect(theManual, SIGNAL(blockDetected(bool, bool, bool)), this, SLOT(blockDetected(bool, bool, bool)));
    connect(theManual, SIGNAL(on_mosaic_loaded(int)), this, SLOT(on_mosaic_loaded(int)));
    connect(theManual, SIGNAL(on_step_changed(int)), this, SLOT(on_step_changed(int)));
    connect(theManual, SIGNAL(on_description_done()), this, SLOT(on_description_done()));

    connect(theManual, SIGNAL(frameFinished(cv::Mat)), this, SLOT(updateFrameLabel(cv::Mat)));
    connect(theManual, SIGNAL(createdExpectedBlockPath(QString, QString, QString)), this, SLOT(updateExpectedBlockLabel(QString, QString, QString)));
    connect(theManual, SIGNAL(createdDetectedBlockPath(QString, bool)), this, SLOT(updateDetectedBlockLabel(QString, bool)));
    connect(creatorThread, SIGNAL(started()), theManual, SLOT(runManual()));

    connect(theManual, SIGNAL(finished()), creatorThread, SLOT(quit()));
    connect(theManual, SIGNAL(finished()), theManual, SLOT(deleteLater()));

    connect(creatorThread, SIGNAL(finished()), creatorThread, SLOT(deleteLater()));

    //Start the thread which holds theManual object
    creatorThread->start();

}

//WHEN SAVE BUTTON IS CLICKED
void ConstructAR::on_newFile_clicked()
{
    //Set the path for the save file by prompting the user to specify a location from a dialog
    QString fileSavePath = QFileDialog::getSaveFileName(this, tr("Choose save destination"), "", "Lego Mosaic(*.xml)");

    //If the newly created path is not empty
    if(!fileSavePath.isEmpty()){
        //Construct the path for the .xml and .jpg paths and emit startTheManual signal
        QString mosaicPictureSavePath;
        QFileInfo fileInfo(fileSavePath);
        mosaicName = fileInfo.baseName();

        mosaicPictureSavePath = fileInfo.path() + "/" + fileInfo.completeBaseName() + ".jpg";
        std::string stdMosaicPictureSavePath = mosaicPictureSavePath.toStdString();
        std::string stdFilePath = fileSavePath.toStdString();

        emit startTheManual(stdFilePath, stdMosaicPictureSavePath, true);


        showCreateUI();
    }
}

//WHEN OPEN FILE BUTTON IS CLICKED
void ConstructAR::on_openFile_clicked()
{    
    //Prompt the user to select a file for opening via a dialog and get its path
    QString filePath = QFileDialog::getOpenFileName(
                        this,
                        tr("Open Mosaic"),
                        "",
                        "Lego Mosaic(*.xml)"
    );


    if(!filePath.isEmpty()){
        //Store the loading picture and the initial picture for the detectedBlock label in QImages
        QImage loadingBG(":/Icons/loadingBG.jpg"), initialDB(":/Icons/initialDB.jpg");

        //Get the name of the file from its path and store it in mosaicName
        QFileInfo fileInfo(filePath);
        QString mosaicPictureLoadPath;
        mosaicName = fileInfo.baseName();

        //Construct the path for the .xml and .jpg files and store them in std::string variables
        mosaicPictureLoadPath = fileInfo.path() + "/" + fileInfo.completeBaseName() + ".jpg";

        std::string stdMosaicPictureLoadPath = mosaicPictureLoadPath.toStdString();
        std::string stdFilePath = filePath.toStdString();


        //Load the picture of the mosaic from its path in its QImage
        loadedMosaic = QImage(mosaicPictureLoadPath);


        //Emit signal
        emit startTheManual(stdFilePath, stdMosaicPictureLoadPath, false);


        showRecreateUI();
    }
}

//SLOT FUNCTION EXECUTED ON EACH FRAME CAPTURE
void ConstructAR::updateFrameLabel(cv::Mat frame){
    //Take the frame, convert it from BGR to RGB and store it in rgbCamFrame object
    cv::Mat rgbCamFrame;
    cv::cvtColor(frame, rgbCamFrame, cv::COLOR_BGR2RGB);

    //Construct a QImage object from the Mat object in order to be able to apply it to a Qt widget (label)
    QImage qCamFrame = QImage(rgbCamFrame.data, rgbCamFrame.cols, rgbCamFrame.rows, rgbCamFrame.step, QImage::Format_RGB888);

    //Update the label with the current frame
    ui->frameLabel->setPixmap(QPixmap::fromImage(qCamFrame));
}

//SLOT FUNCTION UPDATING THE EXPECTED BLOCK PICTURE AND TEXT
void ConstructAR::updateExpectedBlockLabel(QString path, QString blockType, QString blockColor){

    //Load the picture from its path to a QImage
    QImage expectedBlock = QImage(path);

    //Update the label which holds the picture with the new picture
    ui->expectedBlockLabel->setPixmap(QPixmap::fromImage(expectedBlock));

    //update the text of the label used to explain what is the picture actually showing
    ui->ebLabel->setText("Next step: " + blockType + ", " + blockColor);
}

//SLOT FUNCTION UPDATING THE DETECTED BLOCK PICTURE AND TEXT
void ConstructAR::updateDetectedBlockLabel(QString _path, bool _isBlockRemoved){

    dbPath = _path;
    isBlockRemoved = _isBlockRemoved;
    QTimer::singleShot(1000, this, SLOT(refreshDetectedBlockLabel()));
}

void ConstructAR::refreshDetectedBlockLabel(){
    //Load picture and update label
    QImage detectedBlock = QImage(dbPath);

    ui->detectedBlockLabel->setPixmap(QPixmap::fromImage(detectedBlock));

    //Update text above picture label to indicate if block is placed or removed
    if(isBlockRemoved){
        ui->dbLabel->setText("Block removed: ");
    }
    else{
        ui->dbLabel->setText("Block placed: ");
    }
}

//FUNCTION HANDLING KEYPRESS EVENTS, USED FOR DEBUGGING
void ConstructAR::keyPressEvent(QKeyEvent *event)
{
    //Check which key is pressed and update the booleans in theManual object accordingly
    if(event->key() == Qt::Key_S)
    {
        emit startTheManual();
    }
    else if(event->key() == Qt::Key_W){
        theManual->willSaveMosaic = true;
    }
    else if(event->key() == Qt::Key_R){
        theManual->willLoadMosaic = true;
    }
    else if(event->key() == Qt::Key_C){
        theManual->willClearVector = true;
    }
    else if(event->key() == Qt::Key_T){
        theManual->willSortBlocks = true;
    }
    else if(event->key() == Qt::Key_D){
        emit startTheManual("LegoModel.xml", "LegoModel.jpg", false);
    }
    else if(event->key() == Qt::Key_Q){
        theManual->willExit = true;
        this->destroy();

        QApplication::quit();
        return;
    }
}

//WHEN SAVE FILE BUTTON IS CLICKED
void ConstructAR::on_saveFile_clicked()
{
    //The only action required is to update the boolean in theManual, the rest is done by theManual object itself
    theManual->willSaveMosaic = true;
}

//WHEN RESET BUTTON IS CLICKED
void ConstructAR::on_reset_clicked()
{
    //Update the content of the infoLabel based on whether user is creating or recreating a mosaic
    if(!theManual->isCreating){
        ui->infoLabel->setText("  Tip: Place a block with a matching type and color in the specified position to start rebuilding.");
    }
    else{
        ui->infoLabel->setText("  Tip: Place a block on the plate to start building.");
    }

    if(theManual->isCreating){
        //Clear widget content
        ui->blocksList->clear();
        ui->totalNumberSteps->clear();
    }


    //Update boolean
    theManual->willClearVector = true;
}


//UPDATES UI FOR CREATING A MOSAIC
void ConstructAR::showCreateUI(){

    //Update text content of labels, enable required buttons, show required widgets and hide widgets that are not needed
    ui->projectTitle->setText("  Creating project | " + mosaicName);
    ui->infoLabel->setText("  Tip: Place a block on the plate to start building.");

    ui->detectedBlockLabel->setGeometry(createMdetectedBlockLabelGeometry);
    ui->dbLabel->setGeometry(createMdbLabelGeometry);

    ui->frameLabel->setPixmap(QPixmap::fromImage(loadingBG));
    ui->detectedBlockLabel->setPixmap(QPixmap::fromImage(initialDB));
    ui->ebLabel->hide();
    ui->expectedBlockLabel->hide();
    ui->frLabel->hide();
    ui->finalResultLabel->hide();
    ui->detectedBlockLabel->show();
    ui->dbLabel->show();
    ui->saveFile->setEnabled(true);
    ui->reset->setEnabled(true);
    ui->shareFile->setEnabled(true);
    ui->blocksList->clear();
    ui->totalNumberSteps->clear();

}

//UPDATES UI FOR RECREATING A MOSAIC
void ConstructAR::showRecreateUI(){

    //Update text content of labels, enable required buttons, show required widgets and hide widgets that are not needed
    ui->projectTitle->setText("  Recreating project | " + mosaicName);
    ui->infoLabel->setText("  Tip: Place a block with a matching type and color in the specified position to start rebuilding.");

    ui->finalResultLabel->setPixmap(QPixmap::fromImage(loadedMosaic));

    ui->saveFile->setDisabled(true);
    ui->reset->setEnabled(true);

    ui->dbLabel->setGeometry(recreateMdbLabelGeometry);
    ui->detectedBlockLabel->setGeometry(recreateMdetectedBlockLabelGeometry);

    ui->frameLabel->setPixmap(QPixmap::fromImage(loadingBG));
    ui->detectedBlockLabel->setPixmap(QPixmap::fromImage(initialDB));

    ui->progressBar->show();
    ui->progressLabel->show();
    ui->frLabel->show();
    ui->finalResultLabel->show();

    ui->detectedBlockLabel->show();
    ui->dbLabel->show();

    ui->expectedBlockLabel->show();
    ui->ebLabel->show();

    ui->blocksList->clear();
    ui->totalNumberSteps->clear();

}

//WHEN SHARE FILE IS CLICKED
void ConstructAR::on_shareFile_clicked()
{
    //Open default mail app with updated subject field and example email recipient
    QDesktopServices::openUrl(QUrl::fromEncoded("mailto:yourrecipient@mail.com?subject=Try%20my%20Lego%20Mosaic"));
}

//WHEN EXIT BUTTON IS CLICKED
void ConstructAR::on_exit_clicked()
{
    //Update boolean
    theManual->willExit = true;

    //Destroy the UI window
    this->destroy();

    //Quit complete application
    QApplication::quit();
    return;
}

//SLOT FUNCTION EXECUTED WHEN BLOCK IS DETECTED
void ConstructAR::blockDetected(bool isCorrect, bool isRemoved, bool isCreating){

    //Update infoLabel content based on whether block is correct, incorrect, placed or removed
    if(isCreating){
        if(!isRemoved){
            ui->infoLabel->setText("  Tip: Block placed, build your mosaic and save it to see details.");
        }
        else{
            ui->infoLabel->setText("  Tip: You removed a block, you can continue building.");
        }
    }
    else{
        if(isRemoved){
            ui->infoLabel->setText("  Tip: You removed a block, you can continue rebuilding.");
        }
        else if(theManual->isMosaicRecreated){
            ui->infoLabel->setText("  Tip: Mosaic successfully rebuilt.");
        }
        else{
            if (isCorrect) {
                ui->infoLabel->setText("  Tip: Correct block placed. You can proceed to the next step.");
            }
            else{
                ui->infoLabel->setText("  Tip: Incorrect block placed. Please remove it and place a block with matching type and color in the specified position.");
            }
        }
    }
}


//SLOT FUNCTION EXECUTED WHEN USER MAKES A STEP
void ConstructAR::on_step_changed(int step){
    //Update the value of the progress bar with the current step
    QString progressString;
    ui->progressBar->setValue(step);

    //Construct the text for the progress bar label
    if(!theManual->isMosaicRecreated){

        if(step > 9){
            progressString = "Step " + QString::number(step+1) + QString::fromStdString(" of ") + QString::number(ui->progressBar->maximum());
        }
        else{
            progressString = " Step " + QString::number(step+1) + QString::fromStdString(" of ") + QString::number(ui->progressBar->maximum());
        }

        //Update label text
        ui->progressLabel->setText(progressString);
    }

}


//SLOT FUNCTION
void ConstructAR::on_mosaic_loaded(int maximum){
    //Set maximum value for progress bar (total number of steps)
    ui->progressBar->setMaximum(maximum);

    //Update label text and show details for mosaic
    ui->progressLabel->setText("Step 1 of " + QString::number(maximum));
    toggleMosaicDescription(true);
}

//SLOT FUNCTION WHICH EXECUTES AFTER A MOSAIC IS SAVED AND DESCRIPTION IS CREATED
void ConstructAR::on_description_done(){
    //Show the details of the mosaic to the user
    toggleMosaicDescription(true, theManual->isCreating);
}

//The function which handles the display of the mosaic description
void ConstructAR::toggleMosaicDescription(bool show, bool isCreating){
    //If it should hide the description or show it
    if(!show){
        ui->blocksList->hide();
        ui->blocksUsedNeeded->hide();
        ui->detailsFrame->hide();
        ui->detailsLabel->hide();
        ui->totalNumberSteps->hide();
        ui->totalNumberStepsLabel->hide();
    }
    else{
        QString blockListString;

        std::vector<LegoBlock> theDescription = theManual->theMosaicDescription;

        //Compile the description text from the vector of LegoBlock objects
        for(size_t i=0;i<theDescription.size(); i++){
            LegoBlock currentBlock = theDescription[i];
            int occurrences = currentBlock.occurrences;
            QString listLine;
            if( i == theDescription.size() - 1 ){
                listLine = "(" + QString::number(occurrences) + "x) " + QString::fromStdString(currentBlock.type) + ", " + QString::fromStdString(currentBlock.color) + ".";
            }
            else if(i % 2 == 1){
                listLine = "(" + QString::number(occurrences) + "x) " + QString::fromStdString(currentBlock.type) + ", " + QString::fromStdString(currentBlock.color) + ";\n";
            }
            else{
                listLine = "(" + QString::number(occurrences) + "x) " + QString::fromStdString(currentBlock.type) + ", " + QString::fromStdString(currentBlock.color) + ";   ";
            }

            blockListString += listLine;
        }

        //Update widgets based on whether the user is creating or recreating mosaic
        if(isCreating){
            ui->blocksList->setGeometry(createMblockListGeometry);
            ui->blocksUsedNeeded->setGeometry(createMblocksUsedNeededGeometry);
            ui->detailsFrame->setGeometry(createMdetailsFrameGeometry);
            ui->detailsLabel->setGeometry(createMdetailsLabelGeometry);
            ui->totalNumberSteps->setGeometry(createMtotalNumberStepsGeometry);
            ui->totalNumberStepsLabel->setGeometry(createMtotalNumberStepsLabelGeometry);

            ui->blocksUsedNeeded->setText("Blocks used: ");
            ui->totalNumberSteps->setText(QString::number(theManual->theMosaic.size()) + " blocks");
        }
        else{
            ui->blocksList->setGeometry(recreateMblockListGeometry);
            ui->blocksUsedNeeded->setGeometry(recreateMblocksUsedNeededGeometry);
            ui->detailsFrame->setGeometry(recreateMdetailsFrameGeometry);
            ui->detailsLabel->setGeometry(recreateMdetailsLabelGeometry);
            ui->totalNumberSteps->setGeometry(recreateMtotalNumberStepsGeometry);
            ui->totalNumberStepsLabel->setGeometry(recreateMtotalNumberStepsLabelGeometry);

            ui->blocksUsedNeeded->setText("Blocks needed: ");
            ui->totalNumberSteps->setText(QString::number(theManual->theLoadedMosaic.size()) + " steps");
        }

        ui->blocksList->setText(blockListString);
        ui->blocksList->show();
        ui->blocksUsedNeeded->show();
        ui->detailsFrame->show();
        ui->detailsLabel->show();
        ui->totalNumberSteps->show();
        ui->totalNumberStepsLabel->show();

    }
}

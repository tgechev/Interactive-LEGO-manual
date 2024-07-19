#ifndef THEMANUAL_H
#define THEMANUAL_H

#include <QObject>
#include <QCoreApplication>
#include "lego.h"

//Header file for the 'TheManual' class which handles the human-computer interaction, detects blocks, hands and more.
class TheManual : public QObject
{
    Q_OBJECT
public:
    TheManual();
    ~TheManual();
    //Booleans used for determining whether the user has initiated an action
    bool willCreateMosaic = false;
    bool willRecreateMosaic = false;
    bool willExit  = false;
    bool willSortBlocks = false;
    bool willClearVector = false;
    bool willSaveMosaic = false;
    bool willLoadMosaic = false;


    //Boolean used to check if a mosaic is completely recreated
    bool isMosaicRecreated = false;
    //Boolean used for determining whether the program is counting block occurrences
    bool isCreating;

    //Vectors of Lego blocks representing the mosaic currently being built, the mosaic loaded from a file, and the mosaic description.
    std::vector<LegoBlock> theMosaic, theLoadedMosaic, theMosaicDescription;


    //An overloaded '<<' operator which uses qDebug for printing lego block information to the Qt Creator console
    friend QDebug operator<<(QDebug debug, const LegoBlock& theBlock);

    //Function which waits 3 seconds for the Auto White Balancing feature of the camera to adjust the frame
    void waitForAWB();

public slots:
    //A slot which executes once the 'theManual' object is run by its thread
    void runManual();
    //A slot which captures the event emitted by the signal with the same name in the ConstructAR class
    void startTheManual(std::string path = "LegoModel.xml", std::string modelPicturePath = "LegoModel.jpg", bool isCreating = true);

signals:
    //Signal which fires once a frame is captured and stored in a Mat object, it is captured by updateFrameLabel slot in ConstructAR class
    void frameFinished(cv::Mat frame);

    //Signals which fire once the paths for the pictures of the expected and detected blocks are created
    //They are needed so that the program can locate and display the correct picture of aforementioned blocks to the user
    void createdExpectedBlockPath(QString path, QString blockType, QString blockColor);
    void createdDetectedBlockPath(QString path, bool isBlockRemoved);

    //A signal fired once the createMosaic() function in the 'theManual' object has finished execution, meaning that the program will close
    void finished();

    //A signal fired once a block is detected, it is captured by the slot with the same name in the ConstructAR class
    void blockDetected(bool isCorrect, bool isRemoved, bool isCreating);

    void on_mosaic_loaded(int maximum);
    void on_step_changed(int step);
    void on_description_done();

private:
    //PARAMETERS
    const cv::String filename = "LegoModel.xml";            //filename variable holding the name of the savefile, default value is LegoModel.xml

    //Strings holding information about the expected and detected blocks
    QString ebPath, dbPath, ebType, ebColor;

    //A pointer to the Camera object
    Camera* cam;

    //Mat objects used for storing frames with different purposes
    //camFrame - the raw frame captured by the camera
    //createdMosaicFrame - a frame captured when the user saves their complete model
    //oldFrame - a frame, which is considered old, meaning the frame right before the user places a block on the plate
    //currentFrame - a frame, which is considered the current frame, the frame captured right after the user has placed a block
    //diffFrame - the frame which stores the difference between the old and the current frames, meaning the last placed block on a black background
    cv::Mat camFrame, createdMosaicFrame, oldFrame, currentFrame, diffFrame;

    //A counter used for making the bounding box of an incorrectly placed block blink
    byte blinkCounter = 0;

    //Vectors holding the block parameters needed to categorize a block and the different lego colors
    std::vector<BlockParams> bParams;
    std::vector<BlockColors> bColors;

    //LegoBlock objects holding blocks with a specific purpose, names are self-explanatory
    LegoBlock theBlock, blockToPlace, incorrectBlock, correctBlock;


    //int variables indicating the state the program is in ( (re)createSwitch ), the index of the block to remove and the index of the block to place
    int createSwitch = 0, recreateSwitch = 0, blockToRemoveIndex = -1, blockToPlaceIndex = 0;

    //Booleans for determining if something has occurred or if the user is either creating or recreating a mosaic
    bool gameStarted = false;
    bool handDetected = false;
    bool blockRemoved = false;
    bool createModel = false;
    bool drawBB = false;

    //Variables used to set for how long should the program wait before capturing an oldFrame or currentFrame
    float secondsInterval = 1, waitForHandSec = 1;

    //Variables which enable the program to wait exactly X amount of time (seconds)
    float timeDelta;    //Stores the time since the last wait started
    clock_t clk;        //stores time in program (CPU specific) ticks

    clock_t temp;       //Stores the time in ticks of a single loop iteration

    //Variables which hold the path to the .xml file for a mosaic and the .jpg file of the final result
    std::string mosaicPath, mosaicPicturePath;



    //FUNCTIONS
    void loadParams();

    void updateFrame();

    void prepareCreatedMosaicFrame();

    //Function which runs when user is creating their own mosaic
    void createMosaic();

    //Function which runs when user is rebuilding a mosaic
    void recreateMosaic();

    void saveFramePerInterval();

    void updateBlocks();

    void updateCorrectBlock();

    void updateIncorrectBlock();

    cv::Mat removeBackground(cv::Mat);

    bool detectHand(cv::Mat);

    //This function takes a frame (Mat object), detects the block by calling categorizeBlob() internally and returns a LegoBlock object
    LegoBlock detectBlock(cv::Mat&, int);

    //This function takes a Blob (vector of points), categorizes it and returns a LegoBlock object
    LegoBlock categorizeBlob(std::vector<cv::Point>);

    //Function which sorts the blocks from top left to bottom right by using std::sort
    void sortBlocks();

    //Function called when user saves a mosaic
    void Save();

    //Function called when a mosaic is loaded
    void Load();

    //Function returning a boolean based on if the user is either already creating or recreating a mosaic
    bool isRunning();

    //Function which resets the (re)creation process
    void Reset(bool isFromStart = false);

    //Function executed when program is initially ran, it initializes the camera and calls waitForAWB internally
    void prepareCamera();

};

#endif // THEMANUAL_H

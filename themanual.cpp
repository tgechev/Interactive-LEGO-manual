#include "themanual.h"
#include <QThread>
#include <QTime>

//Implementation of the 'theManual' class, representing the whole system

//Global 'std::set' container of 'LegoBlock' objects. It is used to construct the description of every mosaic
//which says how many blocks of each type are used in a mosaic. An 'std::set' is used for this purpose since it does not allow duplicate
//objects to be inserted in it.
std::set<LegoBlock, decltype(&isBlockDifferent)> theLegoSet(&isBlockDifferent);

//Class constructor
TheManual::TheManual()
{
    //Instantiate a new Camera object with camID = 1, which refers to the uEye camera connected via USB.
    cam = new Camera(1);
}

//Class destructor. Called when 'theManual' object is destroyed
TheManual::~TheManual()
{
    //Delete the 'cam' object explicitly
    delete cam;
}

//Overloaded '<<' operator to print out details of a LegoBlock object to the Qt console
QDebug operator<<(QDebug debug, const LegoBlock &theBlock)
{
    //Disable auto quotes and spaces when printing
    debug.noquote();
    debug.nospace();
    //Print out details of a block by first converting 'std::string' parameters to 'QString' objects
    int tlX = theBlock.boundingBox.x, tlY = theBlock.boundingBox.y;
    debug << "==============================================\n";
    debug << "Block color: " << QString::fromStdString(theBlock.color) << "\n";
    debug << "Block type: " << QString::fromStdString(theBlock.type)<< "\n";
    debug << "Block orientation: " << QString::fromStdString(theBlock.orientation)<< "\n";
    //Print out coordinates of top left corner of a block's bounding box as 'int' variables
    debug << "Block Top Left Point: " << "["<<tlX<<", "<<tlY<<"]" << "\n";
    debug << "=============================================="<< "\n";

    return debug;
}

//The function called when the system captures a frame
void TheManual::updateFrame(){

    //Save the frame in the camFrame 'Mat' object
    camFrame = cam->getFrame();

    //Flip and resize it so that it appears correctly in the program's window.
    //Flip is necessary because of how the camera is mounted to the stand
    cv::flip(camFrame, camFrame, -1);
    cv::resize(camFrame, camFrame, cv::Size(), 0.8, 0.8);
}

//Executed when the user saves their mosaic
//The frame captured from this function is saved as the 'final result' for a mosaic
void TheManual::prepareCreatedMosaicFrame(){
    //Save the frame in a 'Mat' object for the purpose
    createdMosaicFrame = cam->getFrame();

    //Again flip and resize so that it matches its label size in the program window
    cv::flip(createdMosaicFrame, createdMosaicFrame, -1);
    cv::resize(createdMosaicFrame, createdMosaicFrame, cv::Size(), 0.3, 0.3);
}

//Executes right after the user chooses to create or recreate a mosaic
//Waits for 3 seconds and captures frames in order for the camera to adjust its Auto White Balance function
void TheManual::waitForAWB(){

    while (timeDelta < 3) {

        qDebug() << "waiting...";
        temp = clock() - clk;
        clk = clock();
        timeDelta += (float)((float)temp / CLOCKS_PER_SEC);

        updateFrame();
    }
}

//Executed initially when the program is started.
//This function loads all the necessary parameters for a block
//HSV ranges for detection, Block type names and enums, Color names, enums and Scalars
//Block widths and heights
void TheManual::loadParams(){
    //Parameters are first written here in arrays and vectors
    int blockWidth[] = { 10, 20, 30, 40, 60, 80, 20, 30, 40, 60 };
    int blockHeight[] = { 10, 10, 10, 10, 10, 10, 20, 20, 20, 20 };
    std::vector<BlockTypes> enumVec = { BS_1x1, BS_1x2, BS_1x3, BS_1x4, BS_1x6, BS_1x8, BS_2x2, BS_2x3, BS_2x4, BS_2x6 };
    std::vector<std::string> typeStrings = { "1x1", "1x2" , "1x3" , "1x4" , "1x6" , "1x8" , "2x2" , "2x3" , "2x4" , "2x6" };
    std::vector<Colors>	colorsVec = { Blue, Green, LightBlue, LightGreen, Orange, Purple, Red, White, Yellow, Black };
    std::vector<cv::Scalar> colorScalarVec = {cv::Scalar(255, 0, 0), cv::Scalar(47, 82, 19), cv::Scalar(213, 127, 87), cv::Scalar(40, 168, 119),
                                         cv::Scalar(34, 104, 242), cv::Scalar(119, 45, 64), cv::Scalar(30, 39, 172), cv::Scalar(255, 255, 255), cv::Scalar(0, 255, 255), cv::Scalar(0, 0, 0)};
    std::vector<std::string> colorNames = {"Blue", "Green", "LightBlue", "LightGreen", "Orange", "Purple", "Red", "White", "Yellow",  "Black"};

    int minHs[] = { 105, 50, 90, 30, 0, 120, 0, 10, 11, 20 };
    int maxHs[] = { 121, 100, 150, 100, 15, 150, 10, 180, 106, 180 };
    int minSs[] = { 175, 100, 100, 110, 100, 115, 171, 10, 80, 10 };
    int maxSs[] = { 230, 255, 155, 255, 255, 255, 255, 95, 255, 130 };
    int minVs[] = { 130, 40, 160, 110, 210, 70, 70, 180, 175, 10 };
    int maxVs[] = { 180, 255, 255, 255, 255, 255, 200, 255, 255, 50 };

    //Here the function iterates over the enumVec, which contains 10 elements, which is the amount of sizes and colors available
    //At each iteration Parameters are loaded in two vectors named 'bParams' and 'bColors',
    //the first containing all the block sizes, labels and enums, the second containing all the block colors as HSV ranges,
    //labels, enums and Scalars
    for (size_t i = 0; i < enumVec.size(); i++) {
        BlockParams param;
        param.Width = blockWidth[i];
        param.Height = blockHeight[i];
        param.enumType = enumVec[i];
        param.type = typeStrings[i];
        bParams.push_back(param);

        BlockColors bColor;
        bColor.enumColor = colorsVec[i];
        bColor.color = colorNames[i];
        bColor.colorScalar = colorScalarVec[i];
        bColor.minH = minHs[i];
        bColor.maxH = maxHs[i];
        bColor.minS = minSs[i];
        bColor.maxS = maxSs[i];
        bColor.minV = minVs[i];
        bColor.maxV = maxVs[i];
        bColors.push_back(bColor);
    }
}

//Executed by the thread 'theManual' object belongs to
void TheManual::runManual()
{


    //Parameters are loaded first
    loadParams();

    //Endless loop which checks whether the user has initiated an action and which runs either the 'createState()'
    //or the 'recreateState()' functions based on what the user has chosen
    while(true){
        //Process events, necessary so that signals can be passed between 'theManual' object and the 'ConstructAR' object (main window)
        QCoreApplication::processEvents();

        //When the user has chosen to create their own mosaic
        if (willCreateMosaic) {
            qDebug() << "Creating mosaic.";
            timeDelta = 0;

            //Increment 'createSwitch' which is checked in the 'createState()' function in order to run it
            createSwitch++;
            //Indicate that a model is created and not recreated
            createModel = true;

            //Reset boolean
            willCreateMosaic = false;
        }
        //If the user will recreate an already existing mosaic
        else if (willRecreateMosaic) {
            qDebug() << "Recreating mosaic.";
            //Load the mosaic chosen by the user
            Load();

            //Indicate that a mosaic is being recreated
            createModel = false;

            timeDelta = 0;
            //Increment 'recreateSwitch' which is checked in recreateState() function in order to run the program
            recreateSwitch++;
            willRecreateMosaic = false;
        }
        //If the user wants to exit the program break out from the loop
        else if (willExit) {
            break;
        }

        //If the blocks are to be sorted from top left to bottom right. Not used in the final version, left for debugging
        else if (willSortBlocks) {
            qDebug() << "Blocks are to be sorted";

            willSortBlocks = false;
        }

        //If the list of placed blocks by the user has to be cleared. Used when non-existing blocks are detected
        else if (willClearVector) {
            Reset();

            willClearVector = false;
        }
        //If the user wants to save their mosaic
        else if (willSaveMosaic) {

            Save();
            qDebug() << "Mosaic saved.";
            willSaveMosaic = false;
        }

        //If the user wants to load a mosaic
        else if (willLoadMosaic) {
            Load();
            qDebug() << "Mosaic loaded.";
            willLoadMosaic = false;
        }

        //If the user is creating a mosaic call createState() if not call recreateState()
        if (createModel) {

            createMosaic();
        }
        else {
            recreateMosaic();
        }


        //Sleep the thread for 30 milliseconds
        QThread::msleep(30);


    }


    //When we have exited the loop, explicitly close the camera
    cam->Close();

    //Emit a finished signal so that threads are closed properly
    emit finished();
}

//When a user creates a mosaic
void TheManual::createMosaic(){

    //Switch statement which determines if the program has to save an 'oldFrame', 'currentFrame' or if it has to detect a block
    switch (createSwitch) {
    //CASE 1: Save a frame per interval which is used as an 'oldFrame' meaning right before the user places a block
    case 1:
        //Take a frame
        updateFrame();
        //See if there is a hand in the frame
        handDetected = detectHand(removeBackground(camFrame));

        //If there are blocks on the plate draw a bounding box around the last placed block to indicate that it is placed
        if (theMosaic.size() > 0) {
            if(drawBB){
                cv::rectangle(camFrame, theMosaic.back().boundingBox, cv::Scalar(84, 255, 121), 2, 8);
            }
        }

        //Emit a frameFinished signal, which is used by the main window to display the frame
        emit frameFinished(camFrame.clone());


        //If a hand is not detected save a frame as the 'oldFrame'
        if (!handDetected) {
            saveFramePerInterval();
        }
        //If a hand is detected go to next case
        else if (handDetected) {
            createSwitch++;
        }
        break;

        //CASE 2: An 'oldFrame' is already saved and a hand has been detected, here the program
        //checks if there is still a hand in the frame, if not it waits for a second and saves the 'currentFrame'
    case 2:

        //Take a frame
        updateFrame();

        //See if there is a hand in the frame
        handDetected = detectHand(removeBackground(camFrame));

        //If there are block on the plate, draw the bounding box around the last one
        if (theMosaic.size() > 0) {
            if(drawBB){
                cv::rectangle(camFrame, theMosaic.back().boundingBox, cv::Scalar(84, 255, 121), 2, 8);
            }
        }

        //Emit frame to the main window
        emit frameFinished(camFrame.clone());

        //If a hand is not detected, meaning that a new block is placed
        if (!handDetected) {

            //If it has waited for less than a second
            //Update the wait timer
            if (timeDelta < 1) {
                temp = clock() - clk;
                clk = clock();
                timeDelta += (float)((float)temp / CLOCKS_PER_SEC);
            }

            //If it has waited for at least a second
            else {

                drawBB = false;
                //Save the 'currentFrame'
                currentFrame = cam->getFrame();
                cv::flip(currentFrame, currentFrame, -1);
                cv::resize(currentFrame, currentFrame, cv::Size(), 0.8, 0.8);

                timeDelta = 0;

                //Go to next case
                createSwitch++;
            }
        }
        break;

        //CASE 3: An 'oldFrame' and a 'currentFrame' have been saved, time to detect the block
    case 3:


        //Take a frame
        updateFrame();

        //Remove the background from the old and current frames
        cv::Mat prevFrame, newFrame;
        prevFrame = removeBackground(oldFrame);
        newFrame = removeBackground(currentFrame);

        //Perform image subtraction, the result is the newly placed block on a black background
        cv::absdiff(prevFrame, newFrame, diffFrame);

        //Detect the block and update the vectors
        updateBlocks();

        //If there are block on the plate, draw a bounding box around the last placed one
        if (theMosaic.size() > 0) {
            if(drawBB){
                cv::rectangle(camFrame, theMosaic.back().boundingBox, cv::Scalar(84, 255, 121), 2, 8);
            }

        }

        //Emit the frame to the main window for display
        emit frameFinished(camFrame.clone());

        //Go back to case 1
        createSwitch = 1;
        break;
    }
}

//When the user recreates a mosaic
void TheManual::recreateMosaic(){

    //Similar switch statement as in createState()
    switch (recreateSwitch) {

    //CASE 1: Save an 'oldFrame'
    case 1:

        //Take the frame
        updateFrame();

        //See if hand is detected
        handDetected = detectHand(removeBackground(camFrame));

        //If there is a block to place
        if(blockToPlace.isEmpty){
            //Take the LegoBlock object from the list at the correct index and store it in the 'blockToPlace' object
            blockToPlace = theLoadedMosaic.at(blockToPlaceIndex);

            //Construct the string for the path of the picture showing the block to place
            ebType = QString::fromStdString(blockToPlace.type);
            ebColor = QString::fromStdString(blockToPlace.color);
            ebPath = ":/Blocks/" + QString::fromStdString(blockToPlace.type) + "/" + QString::fromStdString(blockToPlace.color) + ".png";

            //Emit a signal which takes the path and updates a label which shows a picture of the block that has to be placed
            emit createdExpectedBlockPath(ebPath, ebType, ebColor);

        }

        //If the mosaic is not recreated
        if(!isMosaicRecreated){
            if(drawBB){
                //Draw the bounding box of the next block to place
                cv::rectangle(camFrame, blockToPlace.boundingBox, bColors[blockToPlace.enumColor].colorScalar, 2, 8);
            }

            //If an incorrect block is placed draw a blinking bounding box around it until it is removed
            if (!incorrectBlock.isEmpty && blinkCounter % 2 == 0) {
                cv::rectangle(camFrame, incorrectBlock.boundingBox, cv::Scalar(0, 0, 255), 2, 8);
            }
            blinkCounter++;
        }

        //Emit a signal to update the camera feed
        emit frameFinished(camFrame.clone());

        //If a hand is not detected save an old frame
        if (!handDetected) {
            saveFramePerInterval();
        }
        //If a hand is detected go to next case
        else if (handDetected) {
            recreateSwitch++;
        }
        break;

        //CASE 2: Detect currentFrame
    case 2:

        //Take a frame
        updateFrame();

        //See if hand is detected
        handDetected = detectHand(removeBackground(camFrame));

        //If mosaic is not recreated
        if(!isMosaicRecreated){

            if(drawBB){
                //Draw the bounding box of the next block to place
                cv::rectangle(camFrame, blockToPlace.boundingBox, bColors[blockToPlace.enumColor].colorScalar, 2, 8);
            }

            //If an incorrect block is placed draw a blinking red rectangle around it
            if (!incorrectBlock.isEmpty && blinkCounter % 2 == 0) {
                cv::rectangle(camFrame, incorrectBlock.boundingBox, cv::Scalar(0, 0, 255), 2, 8);
            }
            blinkCounter++;
        }


        //Emit a signal with the frame to display it in the main window
        emit frameFinished(camFrame.clone());

        //If a hand is not detected
        if (!handDetected) {

            //If it has waited less than the interval specified (1 second),
            //wait more
            if (timeDelta < waitForHandSec) {
                temp = clock() - clk;
                clk = clock();
                timeDelta += (float)((float)temp / CLOCKS_PER_SEC);
            }

            //Else save the current frame
            else {

                drawBB = false;
                currentFrame = cam->getFrame();
                cv::flip(currentFrame, currentFrame, -1);
                cv::resize(currentFrame, currentFrame, cv::Size(), 0.8, 0.8);

                timeDelta = 0;
                recreateSwitch++;
            }
        }
        break;

        //CASE 3: Detect the newly placed block
    case 3:

        //Take a frame
        updateFrame();

        //Remove the background of the old and currentFrames
        cv::Mat prevFrame, newFrame;
        prevFrame = removeBackground(oldFrame);
        newFrame = removeBackground(currentFrame);

        //Perform image subtraction and save the result in diffFrame
        cv::absdiff(prevFrame, newFrame, diffFrame);

        //Detect the newly placed block from diffFrame and update the list of blocks
        updateBlocks();

        //If the mosaic is not recreated
        if(!isMosaicRecreated){
            if(drawBB){
                //Draw the bounding box of the next block to place
                cv::rectangle(camFrame, blockToPlace.boundingBox, bColors[blockToPlace.enumColor].colorScalar, 2, 8);
            }

            //If an incorrect block is placed draw a blinking red bounding box around it
            if (!incorrectBlock.isEmpty && blinkCounter % 2 == 0) {
                cv::rectangle(camFrame, incorrectBlock.boundingBox, cv::Scalar(0, 0, 255), 2, 8);
            }
            blinkCounter++;
        }

        //Emit the finished frame to the main window
        emit frameFinished(camFrame.clone());

        //Go back to case 1
        recreateSwitch = 1;
        break;
    }
}

//Executes when the gray background of the frame (the building plate has to be removed from the frame)
cv::Mat TheManual::removeBackground(cv::Mat inputFrame){

    //Declare 'Mat' objects which hold the frame at different steps of the processing
    cv::Mat frame_HSV, frame_threshold, newFrame;

    // Convert from BGR to HSV colorspace, save the result to frame_HSV object
    cv::cvtColor(inputFrame, frame_HSV, cv::COLOR_BGR2HSV);

    //Delete gray background, save result in frame_threshold
    cv::inRange(frame_HSV, cv::Scalar(0, 0, 51), cv::Scalar(180, 105, 191), frame_threshold);

    //At this point the gray building plate is detected and it is white in the threshold frame
    //It has to be black so the values in the frame are inverted with the '~' operator
    frame_threshold = ~frame_threshold;

    //Convert the frame to 8 bit image with 1 channel
    frame_threshold.convertTo(frame_threshold, CV_8UC1);

    //Copy the values from inputFrame to newFrame and use frame_threshold as a mask
    //This results in keeping the lego blocks in the frame and making the background black
    //so that image subtraction works correctly
    inputFrame.copyTo(newFrame, frame_threshold);

    //return the processed frame
    return newFrame;
}

//Executes when the system has to check if a hand is in the FOV
bool TheManual::detectHand(cv::Mat inputFrame){

    //A vector containing blobs from which the program will try to detect a hand
    std::vector<std::vector<cv::Point>> handBlobs;
    int hMinH = 0, hMaxH = 180, hMinS = 68, hMaxS = 140, hMinV = 70, hMaxV = 255;
    //int hMinH = 0, hMaxH = 180, hMinS = 70, hMaxS = 140, hMinV = 160,   hMaxV = 200;
    //Mat objects for holding the thresholded frame and the raw frame
    cv::Mat threshFrame;
    cv::Mat handFrame = inputFrame.clone();
    //Bounding box of the hand
    cv::Rect handBB;

    //Convert the frame to HSV
    cv::cvtColor(handFrame, handFrame, cv::COLOR_BGR2HSV);
    //Try to separate blobs which fall in the HSV range for a hand
    cv::inRange(handFrame, cv::Scalar(hMinH, hMinS, hMinV), cv::Scalar(hMaxH, hMaxS, hMaxV), threshFrame);

    //Find the blobs and put them in a vector
    cv::findContours(threshFrame, handBlobs, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    //If the vector is not empty
    if (handBlobs.size() > 0) {

        //Find a blob with an area bigger of equal to minArea
        //and save its index
        double minArea = 550;
        int blobIndex = -1;
        for (size_t i = 0; i < handBlobs.size(); i++) {

            double currentBlobArea = contourArea(handBlobs[i]);
            if (currentBlobArea >= minArea) {

                blobIndex = (int)i;
                break;
            }
        }
        //If the index is bigger or equal to 0 then a hand is detected
        //it has to return true
        if (blobIndex >= 0) {
            return true;
        }
        //else hand is not detected, return false
        else {
            return false;
        }
    }
}

//Executes if hand is not detected
void TheManual::saveFramePerInterval(){
    //if it has waited for less than the defined interval
    //wait again
    if (timeDelta < secondsInterval) {
            temp = clock() - clk;
            clk = clock();
            timeDelta += (float)((float)temp / CLOCKS_PER_SEC);
    }

    //If it has waited for at least a second and a hand is not detected
    else if (timeDelta >= secondsInterval && !handDetected) {

        drawBB = true;
        //Save the oldFrame
        oldFrame = cam->getFrame();
        cv::flip(oldFrame, oldFrame, -1);
        cv::resize(oldFrame, oldFrame, cv::Size(), 0.8, 0.8);

        timeDelta = 0;
    }
}


//Executes when a correct block is detected
void TheManual::updateCorrectBlock(){
        qDebug() << "Correct block placed.";
        //update the correctBlock object to the detected block
        correctBlock = theBlock;
        qDebug() << correctBlock;
        //put the block in the vector/list
        theMosaic.push_back(theBlock);

        //reset the blockToPlace object for the next detection
        blockToPlace.isEmpty = true;

        //If the last block is placed indicate that the mosaic is recreated
        if(blockToPlaceIndex == theLoadedMosaic.size()-1){
            qDebug() << "mosaic recreated.";
            isMosaicRecreated = true;
            //emit a signal to update the progress bar
            emit on_step_changed(theLoadedMosaic.size());
        }
        //if there are more blocks to place just increment the index
        else if (blockToPlaceIndex < theLoadedMosaic.size()-1) {
            blockToPlaceIndex++;
        }
}

//Executes when an incorrect block is placed
void TheManual::updateIncorrectBlock(){
        qDebug() << "The placed block is incorrect." << endl;
        //update the incorrectBlock object
        incorrectBlock = theBlock;
        qDebug() << incorrectBlock;
        //put the block in the vector
        theMosaic.push_back(theBlock);
        qDebug() << blockToPlace;
}

//Executes after image subtraction is performed in order to detect the block
void TheManual::updateBlocks(){
    //cv::Rect currentBlockBB;
    //try to detected a block by testing for each color
    for (int j = 0; j < 10; j++) {

        //Try to detect a block for the current color and save the object in 'theBlock' object
        theBlock = detectBlock(diffFrame, j);

        //If 'theBlock' is not empty then it has found a block
        if (!theBlock.isEmpty) {

            //Create the path to the picture of the block based on its type and color
            dbPath = ":/Blocks/" + QString::fromStdString(theBlock.type) + "/" + QString::fromStdString(theBlock.color) + ".png";

            //If a mosaic is being recreated
            if (!createModel) {

                //If the list of placed blocks is not empty
                if (theMosaic.size() > 0) {
                    //Go through each block in the list
                    for (size_t i = 0; i < theMosaic.size(); i++) {

                        if (theBlock == theMosaic[i]) {
                            if (theBlock == incorrectBlock) {
                                incorrectBlock.isEmpty = true;
                            }
                            else if(theBlock == correctBlock){

                                if(blockToPlaceIndex > 0){
                                    //qDebug() << "in index check if";
                                    if(!isMosaicRecreated){
                                        blockToPlaceIndex--;
                                    }
                                    else{
                                        isMosaicRecreated = false;
                                    }

                                    if(blockToPlaceIndex == 0){
                                        correctBlock = theLoadedMosaic[blockToPlaceIndex];
                                    }
                                    else{
                                        correctBlock = theLoadedMosaic[blockToPlaceIndex-1];
                                    }

                                }

                            }
                            qDebug() << "The following block was removed: ";
                            qDebug() << theBlock;

                            blockRemoved = true;
                            blockToRemoveIndex = (int)i;
                            break;
                        }
                    }
                    if (blockRemoved) {
                        theMosaic.erase(theMosaic.begin() + blockToRemoveIndex);
                        blockToPlace.isEmpty = true;
                        //blockRemoved = false;
                        emit blockDetected(false, true, false);

                        emit on_step_changed(blockToPlaceIndex);
                    }
                    else if (theBlock == blockToPlace) {
                        updateCorrectBlock();
                        emit blockDetected(true, false, false);

                        if(!isMosaicRecreated){
                            emit on_step_changed(blockToPlaceIndex);
                        }
                    }
                    else if (theBlock != blockToPlace) {
                        updateIncorrectBlock();
                        emit blockDetected(false, false, false);

                        emit on_step_changed(blockToPlaceIndex);
                    }
                }
                else if (theBlock == blockToPlace){
                    updateCorrectBlock();
                    emit blockDetected(true, false, false);

                    emit on_step_changed(blockToPlaceIndex);
                }
                else if (theBlock != blockToPlace) {
                    updateIncorrectBlock();
                    emit blockDetected(false, false, false);

                    emit on_step_changed(blockToPlaceIndex);
                }

                theBlock.isEmpty = true;
                break;
            }
            else {
                if (theMosaic.size() > 0) {

                    for (size_t i = 0; i < theMosaic.size(); i++) {
                        //currentBlockBB = theMosaic[i].boundingBox;

                        if (theBlock == theMosaic[i]) {
                            qDebug() << "The following block was removed: " << endl;
                            qDebug() << theBlock;
                            blockRemoved = true;
                            blockToRemoveIndex = (int)i;
                            break;
                        }
                    }
                    if (blockRemoved) {
                        theMosaic.erase(theMosaic.begin() + blockToRemoveIndex);
                        theBlock.isEmpty = true;
                        emit blockDetected(false, true, true);
                    }
                    else {
                        qDebug() << "Block detected: ";
                        qDebug() << theBlock;
                        theMosaic.push_back(theBlock);
                        theLegoSet.insert(theBlock);
                        emit blockDetected(false, false, true);
                    }
                }
                else {
                    qDebug() << "Block detected: ";
                    qDebug() << theBlock;
                    theMosaic.push_back(theBlock);
                    theLegoSet.insert(theBlock);
                    emit blockDetected(false, false, true);
                }
                //theBlock.isEmpty = true;
                break;
            }
        }
    }

    emit createdDetectedBlockPath(dbPath, blockRemoved);

    blockRemoved = false;
    //isBlockCorrect = true;

    qDebug() <<"blockToPlaceIndex: "<<blockToPlaceIndex;
    qDebug() << "Number of blocks on the plate: " << theMosaic.size();

    //sortBlocks();
}

LegoBlock TheManual::detectBlock(cv::Mat& inputFrame, int colorIndex){

    int minH = bColors[colorIndex].minH;
    int maxH = bColors[colorIndex].maxH;
    int minS = bColors[colorIndex].minS;
    int maxS = bColors[colorIndex].maxS;
    int minV = bColors[colorIndex].minV;
    int maxV = bColors[colorIndex].maxV;

    LegoBlock detectedBlock;
    std::vector<std::vector<cv::Point>> blobs;
    cv::Rect boundingBox;
    cv::Mat frameHSV, thresh;

    cv::cvtColor(inputFrame, frameHSV, cv::COLOR_BGR2HSV);

    //detect color
    cv::inRange(frameHSV, cv::Scalar(minH, minS, minV), cv::Scalar(maxH, maxS, maxV), thresh);

    if (colorIndex == 9) {
        cv::medianBlur(thresh, thresh, 5);

        cv::Mat erodeKernel = getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
        cv::Mat dilateKernel = getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
        cv::Point anchor = cv::Point(-1, -1);
        int iterations = 1;
        //medianBlur(thresh, thresh, 5);

        cv::dilate(thresh, thresh, dilateKernel, anchor, iterations);  //Hit
        cv::erode(thresh, thresh, dilateKernel, anchor, iterations);   //Fit
    }

    //cv::imshow("thresh", thresh);

    cv::findContours(thresh, blobs, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    if (blobs.size() > 0) {
        double largestArea = 60;
        int blobIndex = -1;

        for (size_t i = 0; i < blobs.size(); i++) {

            double currentBlobArea = contourArea(blobs[i]);
            if (currentBlobArea > largestArea) {
                largestArea = currentBlobArea;
                blobIndex = (int)i;
            }
        }

        if (blobIndex >= 0) {

            //Find the block
            detectedBlock = categorizeBlob(blobs[blobIndex]);
            detectedBlock.color = bColors[colorIndex].color;
            detectedBlock.enumColor = bColors[colorIndex].enumColor;
            detectedBlock.isEmpty = false;

            boundingBox = boundingRect(blobs[blobIndex]);
            detectedBlock.boundingBox = boundingBox;
            detectedBlock.topLeftCorner = cv::Point(boundingBox.x, boundingBox.y);
        }
    }

    return detectedBlock;
}

LegoBlock TheManual::categorizeBlob(std::vector<cv::Point> blob){
    LegoBlock theBlock;
    cv::Rect boundingBox = boundingRect(blob);
    int bbWidth = boundingBox.width;
    int bbHeight = boundingBox.height;
    int swap = 0;
    //bool isHorizontal = true;

    std::string orientation = "horizontal";
    Orientation enumOrient = Horizontal;

    if (bbHeight > bbWidth) {
        //isHorizontal = false;
        orientation = "vertical";
        enumOrient = Vertical;
        swap = bbHeight;
        bbHeight = bbWidth;
        bbWidth = swap;
    }
    //cout << "bbWidth: "<<bbWidth << endl;
    //cout << "bbHeight: " <<bbHeight<< endl;
    if (bbHeight < 27) {
        bbHeight = 10;
    }
    else if (bbHeight > 27) {
        bbHeight = 20;
    }
    if (bbWidth < 27) {
        bbWidth = 10;
    }
    else if (bbWidth > 27 && bbWidth < 45) {
        bbWidth = 20;
    }
    else if (bbWidth > 46 && bbWidth < 64) {
        bbWidth = 30;
    }
    else if (bbWidth > 66 && bbWidth < 84) {
        bbWidth = 40;
    }
    else if (bbWidth > 90 && bbWidth < 125) {
        bbWidth = 60;
    }
    else if (bbWidth > 130 && bbWidth < 165) {
        bbWidth = 80;
    }
    for (size_t i = 0; i < bParams.size(); i++) {
        if (bbWidth == bbHeight) {
            theBlock.orientation = "square";
            theBlock.enumOrientation = Square;
        }
        if (bbWidth == bParams[i].Width && bbHeight == bParams[i].Height) {
            theBlock.enumType = bParams[i].enumType;
            theBlock.type = bParams[i].type;
            theBlock.orientation = orientation;
            theBlock.enumOrientation = enumOrient;
            //theBlock.isEmpty = false;
        }
    }

    return theBlock;
}

void TheManual::sortBlocks(){
    QDebug dbgStream = qDebug();
    dbgStream.noquote();
    dbgStream.nospace();
    std::sort(theMosaic.begin(), theMosaic.end(), sortBlockPositions);
    dbgStream << "Sorted blocks: " << endl;

    for (size_t i = 0; i < theMosaic.size(); i++) {
        int tlX = theMosaic[i].boundingBox.x, tlY = theMosaic[i].boundingBox.y;
        dbgStream << "Block number: " << i << endl;
        dbgStream << "Block color: " << QString::fromStdString(theMosaic[i].color) << endl;
        dbgStream << "Block type: " << QString::fromStdString(theMosaic[i].type) << endl;
        dbgStream << "Block orientation: " << QString::fromStdString(theMosaic[i].orientation) << endl;
        dbgStream << "Block Top Left Point: " << "["<<tlX<<", "<<tlY<<"]" << endl;
        dbgStream << "==============================================" << endl;
    }
}

void TheManual::Save(){

    isCounting = true;

    theMosaicDescription.clear();

    for (LegoBlock const& block : theLegoSet)
    {
        int myCount = count(theMosaic.begin(), theMosaic.end(), block);
        /*qDebug() << "The " + QString::fromStdString(block.type) + " " +
                    QString::fromStdString(block.color) + " block occurs " +
                    QString::number(myCount) + " times in the mosaic.";*/

        LegoBlock newBlock = block;
        newBlock.occurrences = myCount;

        theMosaicDescription.push_back(newBlock);
    }

    isCounting = false;

    emit on_description_done();

    prepareCreatedMosaicFrame();

    cv::imwrite(mosaicPicturePath, createdMosaicFrame);

    cv::FileStorage fs(mosaicPath, cv::FileStorage::WRITE);

    fs << "theMosaic" << theMosaic;

    fs << "theMosaicDescription" << theMosaicDescription;

    qDebug() << "The saved mosaic contains: " << endl;

    for (size_t i = 0; i < theMosaic.size(); i++) {
        qDebug() << theMosaic[i];

    }
}

void TheManual::Load(){

    //cv::imread(mosaicPicturePath);
    //qDebug() << "Path from load function: ";
    //qDebug() << QString::fromStdString(mosaicPath);
    cv::FileStorage fs;
    qDebug() << "Reading..." << endl;
    fs.open(mosaicPath, cv::FileStorage::READ);

    if (!fs.isOpened())
    {
        qDebug() << "Failed to open " << QString::fromStdString(filename) << endl;

        return;
    }

    fs["theMosaic"] >> theLoadedMosaic;

    fs["theMosaicDescription"] >> theMosaicDescription;

    qDebug() << "The loaded mosaic is: " << endl;

    for (size_t i = 0; i < theLoadedMosaic.size(); i++) {
        qDebug() << theLoadedMosaic[i];

    }
    emit on_mosaic_loaded(theLoadedMosaic.size());
}

void TheManual::startTheManual(std::string path, std::string moPicPath, bool _isCreating){

    isCreating = _isCreating;

    mosaicPath = path;
    mosaicPicturePath = moPicPath;

    if(isCreating){
        if(isRunning()){
            Reset(true);
        }
        else{
            prepareCamera();
        }
        willCreateMosaic = true;
    }
    else{
        if(isRunning()){
            Reset(true);
        }
        else{
            prepareCamera();
        }
        willRecreateMosaic = true;
    }
}

bool TheManual::isRunning(){
    bool isRunning = false;
    if(createSwitch > 0 || recreateSwitch > 0){
        isRunning = true;
    }

    return isRunning;
}

void TheManual::Reset(bool isFromStart){
    if(isFromStart){
        createSwitch = 0;
        recreateSwitch = 0;

        if(theLoadedMosaic.size() > 0){
            theLoadedMosaic.clear();
        }
        if(!theBlock.isEmpty){
            theBlock.isEmpty = true;
        }
        if(!correctBlock.isEmpty){
            correctBlock.isEmpty = true;
        }
        if(blockToRemoveIndex >= 0){
            blockToRemoveIndex = -1;
        }
    }

    if(theMosaic.size() > 0){
        theMosaic.clear();
    }


    if(!incorrectBlock.isEmpty){
        incorrectBlock.isEmpty = true;
    }
    if(!blockToPlace.isEmpty){
        blockToPlace.isEmpty = true;
    }

    if(blockToPlaceIndex > 0){
        blockToPlaceIndex = 0;
        emit on_step_changed(blockToPlaceIndex);
    }

    isMosaicRecreated = false;

    if(isCreating){
        theMosaicDescription.clear();

        theLegoSet.clear();
    }

}

void TheManual::prepareCamera(){

    cam->Initialize();

    cam->SetROI(32, 0, 1112, 964);

    timeDelta = 0;
    clk = clock();
    waitForAWB();
}


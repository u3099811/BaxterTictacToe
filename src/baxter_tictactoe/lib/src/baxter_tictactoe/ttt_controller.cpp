#include "baxter_tictactoe/ttt_controller.h"

using namespace std;
using namespace baxter_core_msgs;
using namespace baxter_tictactoe;
using namespace geometry_msgs;
using namespace sensor_msgs;
using namespace cv;

/**************************************************************************/
/*                            TTTController                               */
/**************************************************************************/
/*UC changes, add counter to make robot pick up from different areas TODO reset counter at end of game*/

//todo record UPDATE
/* Hard coded way points */ ////////////////Time-----joint state////////////
const double victory_wp[][JOINT_NUM * 2 + 1] =    {
													{1.070346,0.698728248083,0.574859299603,-0.0832184576477,0.773126316211,-0.282252464648,0.913102062946,-0.283019455042,-0.410723355487,0.0479368995667,1.31807299048,1.35488852935,1.24827686472,0.576393280389,-1.89600025166},
													{3.311609,0.69911174328,0.579077746765,-0.0847524384338,0.772742821014,-0.284169940631,0.911184586963,-0.284936931024,-0.635068045459,0.17372332403,1.21031084026,1.10139820444,0.040266995636,2.025004924054,-1.72035945165},
													{4.021760,0.553767063794,-0.699878733673,-0.0080533991272,1.09257781492,-0.155315554596,1.3828836787,-0.152247593024,-0.46901462536,-0.705631161621,-0.413407821863,1.27473803328,0.145344679486,1.00935935728,-0.270364113556},
													{8.020817,0.556835025366,-0.698728248083,-0.00843689432373,1.0948787861,-0.154548564203,1.3828836787,-0.151864097827,-0.468631130164,-0.706014656818,-0.413407821863,1.27473803328,0.144577689093,1.00782537649,-0.270747608752},
													{9.520679,0.50774764021,-0.797670008789,-0.1438106987,1.73263129794,-0.253873820105,1.36907785162,-0.263461200018,-0.592500078644,-0.636602026245,-0.55645153017,0.694126305725,0.213606824469,-0.202101968573,-0.269980618359},
													{12.080919,0.510048611389,-0.797670008789,-0.143043708307,1.73263129794,-0.253873820105,1.36869435643,-0.263844695215,-0.592500078644,-0.63161658869,-0.556068034973,0.694509800922,0.213223329272,-0.201718473376,-0.269980618359},
													{14.030137,0.623563189563,-0.447538894354,-0.069796125769,0.177941771191,0.23891750744,0.166436915295,-0.206320415735,-0.759703984332,-0.443320447192,0.280334988666,1.77673324554,0.133456328394,1.66398565776,0.0839854480408},
													{16.560415,0.620878723187,-0.444087437585,-0.0705631161621,0.179475751978,0.237000031458,0.167970896082,-0.206320415735,-0.759320489136,-0.443703942389,0.280334988666,1.77558275995,0.132305842804,1.66475264815,0.0843689432373},
													{20.080919,0.510048611389,-0.797670008789,-0.143043708307,1.73263129794,-0.253873820105,1.36869435643,-0.263844695215,-0.592500078644,-0.63161658869,-0.556068034973,0.694509800922,0.213223329272,-0.201718473376,-0.269980618359},
													{23.030137,0.623563189563,-0.447538894354,-0.069796125769,0.177941771191,0.23891750744,0.166436915295,-0.206320415735,-0.759703984332,-0.443320447192,0.280334988666,1.77673324554,0.133456328394,1.66398565776,0.0839854480408},
													{27.528050,0.644271930176,-0.168737886475,-0.0782330200928,0.288388387793,0.118116520532,0.30526217644,-0.23124760351,-0.557985510956,-0.552233083008,0.0421844716187,1.56772836343,-2.78570910762,-1.47990796342,-0.770058354639},
													{29.315725,0.735160291754,0.859796230627,-0.101242731885,0.686456401794,-0.312932080371,0.800737970361,-0.311398099585,-0.205936920538,0.141893222717,1.66820410492,2.23807796697,-3.05032079323,-0.364320436707,-0.585597165106},
													{31.512354,0.564504929297,0.706014656818,0.0176407790405,0.768907869049,-1.19612151799,0.522703952875,3.0422673941,0.855577783466,-2.03428654505,-0.314082565961,1.14013121929,0.172189343243,1.49064582892,-3.05875768755}

												};
//--------------------------------------------------------time --------1----------------2---------------3-------------4------------5--------------6---------------7------------------8-------------9-------------10-----------11------------12-------------13---------------14----------
const double wave_wp[][JOINT_NUM * 2 + 1] = {
														{1.602895,0.565655414886,0.711383589569,0.0168737886475,0.767373888263,-1.1945875372,0.52653890484,3.04034991812,-0.439485495227,-0.462111711823,-0.0571407842834,2.0578352246,0.589815612268,1.55353904116,-0.212839834076},
														{6.015908,0.738611748523,0.857878754645,-0.0997087510986,0.684538925812,-0.316000041943,0.803805931934,-0.312932080371,-0.858262249841,0.862097201807,3.05070428842,2.61620423075,-3.04073341331,-0.30756314762,-1.6486458499},
														{8.555666,0.738611748523,0.856728269055,-0.100475741492,0.684155430615,-0.31638353714,0.800737970361,-0.314849556354,-0.821830206171,0.695660286511,3.05185477401,1.16812636864,-3.04840331724,-0.224728185168,-1.64174293636},
														{10.515908,0.738611748523,0.857878754645,-0.0997087510986,0.684538925812,-0.316000041943,0.803805931934,-0.312932080371,-0.858262249841,0.862097201807,3.05070428842,2.61620423075,-3.04073341331,-0.30756314762,-1.6486458499},
														{12.55555,0.738611748523,0.856728269055,-0.100475741492,0.684155430615,-0.31638353714,0.800737970361,-0.314849556354,-0.821830206171,0.695660286511,3.05185477401,1.16812636864,-3.04840331724,-0.224728185168,-1.64174293636},
														{14.55555,0.564888424493,0.708699123193,0.0180242742371,0.768907869049,-1.19612151799,0.525771914447,3.0418838989,-0.807640883899,-0.223577699579,0.197500026215,0.302194214868,-3.04993729803,-0.0260776733643,0.130771862018},
														{16.879484,0.566805900476,0.711767084766,0.0161067982544,0.768140878656,-1.19382054681,0.528072885626,3.03996642292,-0.672267079523,-1.00974285247,0.21897575722,1.21337880183,-0.0368155388672,1.70885459575,-1.25709725424}
											};



//--------------------------------------------------------time --------1----------------2---------------3-------------4------------5--------------6---------------7------------------8-------------9-------------10-----------11------------12-------------13---------------14----------
const double giggle_wp[][JOINT_NUM * 2 + 1] = {
													{3.934641,0.73784475813,0.856728269055,-0.0993252559021,0.683771935419,-0.31638353714,0.800737970361,-0.312932080371,-0.316767032336,-0.481286471649,2.16636436522,2.4229226517,-3.05070428842,-0.19136410307,-0.511199096979}
											   };


TTTController::TTTController(string name, string limb, bool legacy_code, bool no_robot, bool use_forces):
                             ArmCtrl(name, limb, no_robot, use_forces, false, false),
                             r(100), _img_trp(_n), _legacy_code(legacy_code), _is_img_empty(true)
{
    pthread_mutexattr_t _mutex_attr;
    pthread_mutexattr_init(&_mutex_attr);
    pthread_mutexattr_settype(&_mutex_attr, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init(&_mutex_img, &_mutex_attr);

    if (getLimb() == "right")
    {
        XmlRpc::XmlRpcValue hsv_red_symbols;
        ROS_ASSERT_MSG(_n.getParam("hsv_red",hsv_red_symbols), "No HSV params for RED!");
        hsv_red=hsvColorRange(hsv_red_symbols);

        XmlRpc::XmlRpcValue hsv_blue_symbols;
        ROS_ASSERT_MSG(_n.getParam("hsv_blue",hsv_blue_symbols), "No HSV params for BLUE!");
        hsv_blue=hsvColorRange(hsv_blue_symbols);

        XmlRpc::XmlRpcValue tiles_pile_pos;
        ROS_ASSERT_MSG(_n.getParam("tile_pile_position",tiles_pile_pos), "No 3D position of the pile of tiles!");
        tilesPilePosFromParam(tiles_pile_pos);

        XmlRpc::XmlRpcValue board_corner_poss;
        ROS_ASSERT_MSG(_n.getParam("board_corner_poss",board_corner_poss), "No 3D position of the board!");
        boardPossFromParam(board_corner_poss);

        _img_sub = _img_trp.subscribe("/cameras/"+getLimb()+"_hand_camera/image",
                               SUBSCRIBER_BUFFER, &TTTController::imageCb, this);
    }

    	insertAction(ACTION_SCAN,    static_cast<f_action>(&TTTController::scanBoardImpl));
        insertAction(ACTION_PICKUP,  static_cast<f_action>(&TTTController::pickUpTokenImpl));
        insertAction(ACTION_PUTDOWN, static_cast<f_action>(&TTTController::putDownTokenImpl));
        insertAction(ACTION_GESTURE, static_cast<f_action>(&TTTController::playbackJoints));
        ROS_INFO("---------------------------------------");
        startAction(LIST_ACTIONS);

    setHomeConfiguration();
    if (!callAction(ACTION_HOME)) setState(ERROR);
}

bool TTTController::tilesPilePosFromParam(XmlRpc::XmlRpcValue _params)
{
    ROS_ASSERT(_params.getType()==XmlRpc::XmlRpcValue::TypeArray);

    for(int j=0; j<_params.size(); ++j)
    {
        ROS_ASSERT(_params[j].getType()==XmlRpc::XmlRpcValue::TypeDouble);
    }

    //UC changed from original
    _tiles_pile_pos0.x = (double)_params[0];// + OFFSET_X;
    _tiles_pile_pos0.y = (double)_params[1];// + OFFSET_Y;
    _tiles_pile_pos0.z = _params[2];
    _tiles_pile_pos1.x = (double)_params[3]; //+ OFFSET_X;
    _tiles_pile_pos1.y = (double)_params[4]; //+ OFFSET_Y;
    _tiles_pile_pos1.z = _params[2];
    _tiles_pile_pos2.x = (double)_params[5]; //+ OFFSET_X;
    _tiles_pile_pos2.y = (double)_params[6]; //+ OFFSET_Y;
    _tiles_pile_pos2.z = _params[2];
    _tiles_pile_pos3.x = (double)_params[7]; //+ OFFSET_X;
    _tiles_pile_pos3.y = (double)_params[8]; //+ OFFSET_Y;
    _tiles_pile_pos3.z = _params[2];
    _tiles_pile_pos4.x = (double)_params[9]; //+ OFFSET_X;
    _tiles_pile_pos4.y = (double)_params[10]; //+ OFFSET_Y;
    _tiles_pile_pos4.z = _params[2];
    coords[0] = _tiles_pile_pos0;
    coords[1] = _tiles_pile_pos1;
    coords[2] = _tiles_pile_pos2;
    coords[3] = _tiles_pile_pos3;
    coords[4] = _tiles_pile_pos4;
    


    return true;
}

bool TTTController::boardPossFromParam(XmlRpc::XmlRpcValue _params)
{
    ROS_ASSERT(_params.getType()==XmlRpc::XmlRpcValue::TypeStruct);
    _board_corners_poss.clear();

    for (XmlRpc::XmlRpcValue::iterator i=_params.begin(); i!=_params.end(); ++i)
    {
        ROS_ASSERT(i->second.getType()==XmlRpc::XmlRpcValue::TypeArray);
        for(int j=0; j<i->second.size(); ++j)
        {
            ROS_ASSERT(i->second[j].getType()==XmlRpc::XmlRpcValue::TypeDouble);
        }
        // printf("%s %i %i\n", i->first.c_str(), static_cast<int>(i->second[0]),
        //                                        static_cast<int>(i->second[1]));
        if (i->first == "TL" || i->first == "TR" ||
            i->first == "BR" || i->first == "BL")
        {
            geometry_msgs::Point p;
            p.x = (double)i->second[0]+ OFFSET_X;
            p.y = (double)i->second[1]+ OFFSET_Y;
            p.z = i->second[2];

            _board_corners_poss.push_back(p);
        }
    }

    ROS_ASSERT(_board_corners_poss.size() == 4);

    std::string coords_str = "";
    for (size_t i = 0; i < _board_corners_poss.size(); ++i)
    {
        coords_str = coords_str + "[ " + toString(_board_corners_poss[i].x) + " " +
                                         toString(_board_corners_poss[i].y) + " " +
                                         toString(_board_corners_poss[i].z) + "] ";
    }
    ROS_INFO("[%s] Board corners [BL BR TL TR]: %s", getLimb().c_str(), coords_str.c_str());

    // Now that we have the corners, let's compute all the coordinates
    _board_centers_poss.clear();
    _board_centers_poss.resize(9);

    _board_centers_poss[0] =                              _board_corners_poss[2];  // TL
    _board_centers_poss[1] = (_board_corners_poss[2] + _board_corners_poss[3])/2;
    _board_centers_poss[2] =                              _board_corners_poss[3];  // TR
    _board_centers_poss[3] = (_board_corners_poss[0] + _board_corners_poss[2])/2;
    _board_centers_poss[4] = (_board_corners_poss[0] + _board_corners_poss[1] +
                              _board_corners_poss[2] + _board_corners_poss[3])/4;
    _board_centers_poss[5] = (_board_corners_poss[1] + _board_corners_poss[3])/2;
    _board_centers_poss[6] =                              _board_corners_poss[0];  // BL
    _board_centers_poss[7] = (_board_corners_poss[0] + _board_corners_poss[1])/2;
    _board_centers_poss[8] =                              _board_corners_poss[1];  // BR

    ROS_INFO("[%s] Computed board centers [TL TR BL BR]:", getLimb().c_str());

    _board_centers_poss.push_back(coords[0]);
    _board_centers_poss.push_back(coords[1]);
    _board_centers_poss.push_back(coords[2]);
    _board_centers_poss.push_back(coords[3]);
    _board_centers_poss.push_back(coords[4]);
    ROS_INFO("[%s] Tile Pile Position: %g %g %g", getLimb().c_str(), coords[0].x,
        														    coords[0].y, coords[0].z);
    ROS_INFO("[%s] Tile Pile Position vector: %g %g %g", getLimb().c_str(), _board_centers_poss[9].x,
    											 _board_centers_poss[9].y, _board_centers_poss[9].z);




    for (size_t i = 0; i < 3; ++i)
    {
        std::string coords_str = "";
        for (size_t j = 0; j < 3; ++j)
        {
            coords_str = coords_str + "[ " + toString(_board_centers_poss[i*3 + j].x) + " " +
                                             toString(_board_centers_poss[i*3 + j].y) + " " +
                                             toString(_board_centers_poss[i*3 + j].z) + "] ";
        }

        ROS_INFO("[%s] %s", getLimb().c_str(), coords_str.c_str());
    }

    return true;
}

/**************************************************************************/
/*                         PickUpToken                                    */
/**************************************************************************/
bool TTTController::gripToken()
{
    /*UC altered*/
	
    if (_legacy_code == true)
    {
        createCVWindows();
        cv::Point offset(0,0);
        // check if token is present before starting movement loop
        // (prevent gripper from colliding with play surface)
        while(RobotInterface::ok())
        {
            if(computeTokenOffset(offset)) break;

            ROS_WARN_THROTTLE(2,"No token detected by hand camera.");
            r.sleep();
        }

        offset = cv::Point(0,0);
    }

    ros::Time start_time = ros::Time::now();
    double start_z = getPos().z;
    ros::Duration wait_for_grip(2);

    while(RobotInterface::ok())
    {
        double px = 0.0, py = 0.0, pz = 0.0;


        if (_legacy_code == true)
        {
            cv::Point offset(0,0);
            computeTokenOffset(offset);

            // move incrementally towards token
            px = getPos().x - 0.07 * offset.y / 500;  // fixed constant to avoid going too fast
            py = getPos().y - 0.07 * offset.x / 500;  // fixed constant to avoid going too fast
            pz = start_z    - 0.08 * (ros::Time::now() - start_time).toSec();
        }
        else
        {
            px = getPos().x; // current pose
            py = getPos().y; // current pose
            pz = start_z - 0.05 * (ros::Time::now() - start_time).toSec();
        }

        goToPoseNoCheck(px,py,pz,VERTICAL_ORI_L);
        //goToPose(px,py,pz,VERTICAL_ORI_L);

        if(pz < -0.1)
        {
            ROS_WARN("I went too low! Exiting.");

            if (_legacy_code == true) { destroyCVWindows(); }
            gripObject();
            wait_for_grip.sleep();
            return false;
        }

        if(hasCollidedIR("loose")) break;
        r.sleep();
    }

    if (_legacy_code == true) { destroyCVWindows(); }
    gripObject();

    wait_for_grip.sleep();
    return true;
}

bool TTTController::computeTokenOffset(cv::Point &offset)
{
    Mat token(_img_size, CV_8UC1);
    Mat pool (_img_size, CV_8UC1);

    pool = detectPool();
    token = isolateToken(pool);

    bool res = false;

    Contours contours;
    vector<cv::Vec4i> hierarchy;
    findContours(token, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

    token = Mat::zeros(_img_size, CV_8UC1);

    // when hand camera is blind due to being too close to token, go straight down;
    if(contours.size() < 2)
    {
        offset = cv::Point(0,0);
    }
    else if(contours.size() <= 4)
    {
        // find highest and lowest x and y values from token triangles contours
        // to find x-y coordinate of top left token edge and token side length
        int y_min = (contours[0])[0].y;
        int x_min = (contours[0])[0].x;
        int y_max = 0;
        int x_max = 0;

        for (size_t i = 0; i < contours.size(); i++)
        {
            vector<cv::Point> contour = contours[i];
            for (size_t j = 0; j < contour.size(); j++)
            {
                if(y_min > contour[j].y) y_min = contour[j].y;
                if(x_min > contour[j].x) x_min = contour[j].x;
                if(y_max < contour[j].y) y_max = contour[j].y;
                if(x_max < contour[j].x) x_max = contour[j].x;
            }
        }

        // reconstruct token's square shape
        rectangle(token, Rect(x_min, y_min, y_max - y_min, y_max - y_min),
                                          Scalar(255,255,255), CV_FILLED);

        // find and draw the center of the token and the image
        int x_mid = int((x_max + x_min) / 2);
        int y_mid = int((y_max + y_min) / 2);

        int x_trg = int(_img_size.width/2+20);   // some offset to center the tile on the gripper
        int y_trg = int(_img_size.height/2-40);  // some offset to center the tile on the gripper

        circle(token, cv::Point(x_mid, y_mid), 3, Scalar(0, 0, 0), CV_FILLED);
        circle(token, cv::Point(x_trg, y_trg), 3, Scalar(180, 40, 40), CV_FILLED);

        offset.x = x_mid - x_trg;
        offset.y = y_mid - y_trg;

        ROS_DEBUG_THROTTLE(1, "Offset %i %i", offset.x, offset.y);

        res=true;
    }

    imshow("Processed", token);
    waitKey(10);

    return res;
}

cv::Mat TTTController::detectPool()
{
    Mat black = Mat::zeros(_img_size, CV_8UC1);
    Mat   out = Mat::zeros(_img_size, CV_8UC1);

    isolateBlack(black);
    // imshow("black", black);

    vector<cv::Vec4i> hierarchy;
    Contours contours;

    // find outer board contours
    findContours(black, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

    double largest = 0;
    int largest_index = 0;

    // iterate through contours and keeps track of contour w/ largest area
    for (size_t i = 0; i < contours.size(); i++)
    {
        if(contourArea(contours[i], false) > largest)
        {
            largest = contourArea(contours[i], false);
            largest_index = i;
        }
    }

    // contour w/ largest area is most likely the inner board
    vector<cv::Point> contour = contours[largest_index];

    drawContours(out, contours, largest_index, Scalar(255,255,255), CV_FILLED);
    return out;
}

Mat TTTController::isolateToken(Mat pool)
{
    Mat blue = Mat::zeros(_img_size, CV_8UC1);
    Mat out  = Mat::zeros(_img_size, CV_8UC1);

    isolateBlue(blue);    // imshow("blue", blue);

    bitwise_and(blue, pool, out);

    // Some morphological operations to remove noise and clean up the image
    for (size_t i = 0; i < 2; ++i)  erode(out, out, Mat());
    for (size_t i = 0; i < 4; ++i) dilate(out, out, Mat());
    for (size_t i = 0; i < 2; ++i)  erode(out, out, Mat());

    imshow("Rough", out);

    return out;
}

/**************************************************************************/
/*                               ScanBoard                                */
/**************************************************************************/

void TTTController::setDepth(float &dist)
{
    ROS_INFO("Computing depth..");

    geometry_msgs::Point init_pos = getPos();

    ros::Time start_time = ros::Time::now();

    // move downwards until collision with surface
    while(RobotInterface::ok())
    {
        double px = init_pos.x;
        double py = init_pos.y;
        double pz = init_pos.z - (ARM_SPEED+0.2) * (ros::Time::now() - start_time).toSec();

        double ox =   1.0;
        double oy = -0.03;
        double oz =   0.0;
        double ow =   0.0;

        goToPoseNoCheck(px,py,pz,ox,oy,oz,ow);

        if(hasCollidedIR("loose")) break;
        r.sleep();
    }

    // offset to account for height difference between IR camera and tip of vacuum gripper
    dist = init_pos.z - getPos().z + 0.04;
    ROS_INFO("Dist is %g", dist);
}

void TTTController::processImage(float dist)
{
    createCVWindows();
    while(RobotInterface::ok())
    {
        Contours contours;
        vector<cv::Point> centroids, board_corners, cell_to_corner;

        Mat binary, board;

        int board_area;

        isolateBlack(binary);
        isolateBoard(contours, board_area, board_corners, binary, board);

        waitKey(3);

        if(contours.size() == 9)
        {
            setOffsets(board_area, contours, dist, board, centroids);
            // imshow("[ScanBoard] Processed", board);

            if(offsetsReachable())
            {
                ROS_INFO_THROTTLE(2, "[Scan Board] Board is positioned correctly! Proceed with game\n");
                break;
            }
            else if(!offsetsReachable())
            {
                ROS_WARN("[Scan Board] Please move board within reachable zone\n");
                setZone(contours, dist, board_corners, centroids, cell_to_corner);

                // calls to IK solver in setZone takes too long; makes the image update
                // very slow and hard for users to calibrate board position, which is why
                // and inner loop is needed
                ros::Time start = ros::Time::now();
                int interval = 10;
                while(RobotInterface::ok())
                {
                    Mat zone = _curr_img.clone();

                    line(zone, centroids[0] + cell_to_corner[0],
                               centroids[2] + cell_to_corner[1], cv::Scalar(0,0,255), 1);
                    line(zone, centroids[2] + cell_to_corner[1],
                               centroids[8] + cell_to_corner[3], cv::Scalar(0,0,255), 1);
                    line(zone, centroids[0] + cell_to_corner[0],
                               centroids[6] + cell_to_corner[2], cv::Scalar(0,0,255), 1);
                    line(zone, centroids[6] + cell_to_corner[2],
                               centroids[8] + cell_to_corner[3], cv::Scalar(0,0,255), 1);

                    if((ros::Time::now() - start).toSec() > interval)
                    {
                        vector<cv::Point> temp_centroids, temp_board_corners;
                        isolateBlack(binary);
                        isolateBoard(contours, board_area, temp_board_corners, binary.clone(), board);
                        if(contours.size() == 9)
                        {
                            setOffsets(board_area, contours, dist, board, temp_centroids);
                        }

                        if(offsetsReachable()) { break; }
                        interval += 5;
                    }

                    imshow("Rough", zone);
                    waitKey(3);
                    r.sleep();
                }
            }
        }

        imshow("Processed", binary);
    }
    destroyCVWindows();
}

void TTTController::isolateBlack(Mat &output)
{
    Mat gray;
    pthread_mutex_lock(&_mutex_img);
    cvtColor(_curr_img, gray, CV_BGR2GRAY);
    pthread_mutex_unlock(&_mutex_img);
    threshold(gray, output, 55, 255, cv::THRESH_BINARY);
}

void TTTController::isolateBlue(Mat &output)
{
    Mat hsv(_img_size, CV_8UC1);

    pthread_mutex_lock(&_mutex_img);
    cvtColor(_curr_img, hsv, CV_BGR2HSV);
    pthread_mutex_unlock(&_mutex_img);

    output = hsvThreshold(hsv, hsv_blue);
}

void TTTController::isolateBoard(Contours &contours, int &board_area,
                                 vector<cv::Point> &board_corners, Mat input, Mat &output)
{
    output = Mat::zeros(_img_size, CV_8UC1);

    vector<cv::Vec4i> hierarchy; // captures contours within contours

    findContours(input, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

    double largest = 0, next_largest = 0;
    int largest_index = 0, next_largest_index = 0;

    // iterate through contours and keeps track of contour w/ 2nd-largest area
    for (size_t i = 0; i < contours.size(); i++)
    {
        if(contourArea(contours[i], false) > largest)
        {
            next_largest = largest;
            next_largest_index = largest_index;
            largest = contourArea(contours[i], false);
            largest_index = i;
        }
        else if(next_largest < contourArea(contours[i], false) && contourArea(contours[i], false) < largest)
        {
            next_largest = contourArea(contours[i], false);
            next_largest_index = i;
        }
    }

    board_area = contourArea(contours[next_largest_index], false);

    drawContours(output, contours, next_largest_index, Scalar(255,255,255), CV_FILLED, 8, hierarchy);

    findContours(output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

    largest = 0;
    largest_index = 0;

    // iterate through contours and keeps track of contour w/ largest area
    for (size_t i = 0; i < contours.size(); i++)
    {
        if(contourArea(contours[i], false) > largest)
        {
            largest = contourArea(contours[i], false);
            largest_index = i;
        }
    }

    vector<cv::Point> board_outline = contours[largest_index];

    /* Set board corners and board area*/
    double y_min = board_outline[0].y;
    double x_min = board_outline[0].x;
    double y_max = 0;
    double x_max = 0;

    for (size_t i = 0; i < board_outline.size(); i++)
    {
        if(y_min > board_outline[i].y) y_min = board_outline[i].y;
        if(x_min > board_outline[i].x) x_min = board_outline[i].x;
        if(y_max < board_outline[i].y) y_max = board_outline[i].y;
        if(x_max < board_outline[i].x) x_max = board_outline[i].x;
    }

    board_corners.push_back(cv::Point(x_max, y_max));
    board_corners.push_back(cv::Point(x_min, y_max));
    board_corners.push_back(cv::Point(x_max, y_min));
    board_corners.push_back(cv::Point(x_min, y_min));

    // remove outer board contours
    contours.erase(contours.begin() + largest_index);

    for (size_t i = 0; i < contours.size(); i++)
    {
        if(contourArea(contours[i], false) < 200)
        {
            contours.erase(contours.begin() + i);
        }
    }

    for (size_t i = 0; i < contours.size(); i++)
    {
        drawContours(output, contours, i, Scalar(255,255,255), CV_FILLED);
    }
}

bool TTTController::descendingX(vector<cv::Point> i, vector<cv::Point> j)
{
    double x_i = moments(i, false).m10 / moments(i, false).m00;
    double x_j = moments(j, false).m10 / moments(j, false).m00;

    return x_i > x_j;
}

void TTTController::setOffsets(int board_area, Contours contours, float dist, Mat &output, vector<cv::Point> &centroids)
{
    cv::Point center(_img_size.width / 2, _img_size.height / 2);

    circle(output, center, 3, Scalar(180,40,40), CV_FILLED);
    cv::putText(output, "Center", center, cv::FONT_HERSHEY_PLAIN, 0.9, cv::Scalar(180,40,40));

    for (size_t i = contours.size(); i >= 3; i -= 3)
    {
        std::sort(contours.begin() + (i - 3), contours.begin() + i, descendingX);
    }

    _offsets.resize(9);//gesture_t
    centroids.resize(9);

    for (int i = int(contours.size()) - 1; i >= 0; i--)
    {
        double x = moments(contours[i], false).m10 / moments(contours[i], false).m00;
        double y = moments(contours[i], false).m01 / moments(contours[i], false).m00;
        cv::Point centroid(x,y);

        centroids[i] = centroid;

        // cv::putText(output, intToString(i), centroid, cv::FONT_HERSHEY_PLAIN, 0.9, cv::Scalar(180,40,40));
        // circle(output, centroid, 2, Scalar(180,40,40), CV_FILLED);
        line(output, centroid, center, cv::Scalar(180,40,40), 1);

        _offsets[i].x = (centroid.y - center.y) * 0.0025 * dist + 0.04;
        _offsets[i].y = (centroid.x - center.x) * 0.0025 * dist;
        _offsets[i].z = dist - 0.065;
    }
}

void TTTController::setZone(Contours contours, float dist, vector<cv::Point> board_corners,
                            vector<cv::Point> c, vector<cv::Point> &cell_to_corner)
{
    cell_to_corner.resize(4);

    // calculate offset between the center of corner cells and the corners of the board
    cell_to_corner[0] = cv::Point(board_corners[0].x - c[0].x, board_corners[0].y - c[0].y);
    cell_to_corner[1] = cv::Point(board_corners[1].x - c[2].x, board_corners[1].y - c[2].y);
    cell_to_corner[2] = cv::Point(board_corners[2].x - c[6].x, board_corners[2].y - c[6].y);
    cell_to_corner[3] = cv::Point(board_corners[3].x - c[8].x, board_corners[3].y - c[8].y);

    // if the centroid of a corner cell is reachable,
    // iterate and check if a location 10 pixels further from arm is still reachable
    // to establish a boundary of how far Baxter's arm can reach
    while(pointReachable(c[0], dist)) {c[0].x += 10.0;}
    while(pointReachable(c[2], dist)) {c[2].x -= 10.0;}
    while(pointReachable(c[6], dist)) {c[6].x += 10.0;}
    while(pointReachable(c[8], dist)) {c[8].x -= 10.0;}

    // if the centroid of a corner cell is unreachable,
    // iterate and check if a location 10 pixels closer is reachable
    while(!pointReachable(c[0], dist)) {c[0].x -= 5.0;}
    while(!pointReachable(c[2], dist)) {c[2].x += 5.0;}
    while(!pointReachable(c[6], dist)) {c[6].x -= 5.0;}
    while(!pointReachable(c[8], dist)) {c[8].x += 5.0;}
}

bool TTTController::offsetsReachable()
{
    for (size_t i = 0; i < NUMBER_OF_CELLS; i++)
    {
        double px = getPos().x + _offsets[i].x;
        double py = getPos().y + _offsets[i].y;
        double pz = getPos().z - _offsets[i].z;

        vector<double> joint_angles;
        if (!computeIK(px,py,pz,VERTICAL_ORI_L,joint_angles))
        {
            ROS_INFO("Offset number %lu not reachable", i);
            return false;
        }
    }
    return true;
}

bool TTTController::pointReachable(cv::Point centroid, float dist)
{
    // convert image location into real world pose coordinates
    cv::Point center(_img_size.width / 2, _img_size.height / 2);

    geometry_msgs::Point offset;

    offset.x = (centroid.y - center.y) * 0.0025 * dist + 0.04;
    offset.y = (centroid.x - center.x) * 0.0025 * dist;
    offset.z = dist - 0.085;

    double px = HOVER_BOARD_X + offset.x;
    double py = HOVER_BOARD_Y + offset.y;
    double pz = HOVER_BOARD_Z - offset.z;

    vector<double> joint_angles;
    return computeIK(px,py,pz,VERTICAL_ORI_L,joint_angles);
}

/**************************************************************************/
/*                               MISC                                     */
/**************************************************************************/

bool TTTController::createCVWindows()
{
    namedWindow("Hand Camera", WINDOW_NORMAL);
    namedWindow(      "Rough", WINDOW_NORMAL);
    namedWindow(  "Processed", WINDOW_NORMAL);
    resizeWindow("Hand Camera", 480, 300);
    resizeWindow(      "Rough", 480, 300);
    resizeWindow(  "Processed", 480, 300);
    moveWindow("Hand Camera", 10,  10);
    moveWindow(      "Rough", 10, 370);
    moveWindow(  "Processed", 10, 720);
    waitKey(10);

    return true;
}

bool TTTController::destroyCVWindows()
{
    destroyWindow("Hand Camera");
    destroyWindow("Processed");
    destroyWindow("Rough");

    return true;
}

bool TTTController::goHome()
{
    return ArmCtrl::goHome();
}

void TTTController::setHomeConfiguration()
{
    if (getLimb() == "left")
    {
        if (_legacy_code == true)
        {
            setHomeConf(  0.561820462921,0.703330190442,0.0199417502197,0.770441849835,-1.19803899397,0.521553467285,3.04380137488);
            ROS_INFO("positioning working1");
        }
        else
        {
            //setHomeConf( -0.21, 1.59, 0.81, 0.081, -2.93, -1.16, -0.34);
        	setHomeConf( 0.561820462921,0.703330190442,0.0199417502197,0.770441849835,-1.19803899397,0.521553467285,3.04380137488);
            ROS_INFO("positioning working2");
        }
    }
    else if (getLimb() == "right")
    {
        setHomeConf(  -0.116966034943,-0.709849608783,-0.207854396521,1.64711186911,0.4203107354,1.40704387608,-0.341694220111);
        ROS_INFO("positioning working3");
    }
}


bool TTTController::startAction(string a, int o, bool block)
{
    baxter_collaboration_msgs::DoAction::Request  req;
    baxter_collaboration_msgs::DoAction::Response res;
    req.action = a;
    req.objects.push_back(o);

    //[UC] edition
    if(block)
    	serviceCb(req,res);
    else
    	unblocking_serviceCB(req, res);

    return res.success;
}

void TTTController::joinAction()
{
	// wait until the internal thread set the internal state 'DONE'
	ros::Rate r(THREAD_FREQ);
	    while( ros::ok() && ( int(getState()) != START   &&
	                          int(getState()) != ERROR   &&
	                          int(getState()) != DONE      ))
	    {
	        if (ros::isShuttingDown())
	        {
	            setState(KILLED);
	            return;
	        }

	        if (getState() == KILLED)
	        {
	        	// go home
	            recoverFromError();
	        }

	        r.sleep();
	    }

	    if ( int(getState()) == START   ||
	         int(getState()) == DONE      )
	    {
	    	ROS_INFO("[%s] Action succeed \n", getLimb().c_str());
	    }

	    if (getState() == ERROR)
	    {
	        ROS_INFO("[%s] Action failed. ERROR: %s \n", getLimb().c_str(), getSubState().c_str());
	    }

	    // Wait until internal thread exit
	    joinInternalThread();
	    return;
}

bool TTTController::playbackJoints()
{
	TTTController::gesture_t id = (TTTController::gesture_t)getObjectID();
	ROS_INFO("Playing gesture %d..", id);
	const double (* wps)[JOINT_NUM * 2 + 1];
	int wp_size = 0;
	double start_time;
	//vector<double> next_wp;
	int start_id;

	switch (id)
	{
		case wave:
			wps = wave_wp;
			wp_size = ARRAY_SIZE(wave_wp);
			break;
		case victory:
			wps = victory_wp;
			wp_size = ARRAY_SIZE(victory_wp);
			break;
		case giggle:
			wps = giggle_wp;
			wp_size = ARRAY_SIZE(giggle_wp);
			break;
		default:
			ROS_WARN("unknown gesture id %d", id);
			return false;
	}

	if(getLimb() == "left")
		start_id = 1;
	else
		start_id = 8;


    // set the start time at the beginning of each gesture
	start_time = ros::Time::now().toSec();
	ros::Rate rate(100);

	for(int j = 0; j < wp_size; j++)
	{
		vector<double> next_wp;
		// make a vector, skip time in the 1st element
		for(int i = start_id; i < JOINT_NUM + start_id; i++)
		{
			next_wp.push_back(wps[j][i]);
			//ROS_INFO("Reached Line i=%i,j=%i",i,j);
		}

		// Keep sending message to limb topic until position reached or timeout
		while (RobotInterface::ok())
		{
			// Move the arm, return when goal gesture reached.
			goToJointConfNoCheck(next_wp);
			// go to the next way point if goal reached
			if(isConfigurationReached(next_wp, "strict"))
			{
				ROS_INFO("Reached way point %i", j);
				break;
			}
			// go to the next way point if time out
			if(ros::Time::now().toSec() - start_time > wps[j][0])
			{
				ROS_WARN("Time out when playing way point %d in gesture %d", j, id);
				break;
			}

			// send message at 100Hz
			rate.sleep();
		}
	}
	// Going back to home position
	goHome();

	ROS_INFO("Finished gesture %d..", id);
	return true;
}

bool TTTController::hoverAboveBoard()
{
    ROS_DEBUG("Hovering above board..");
    // return goToPose(HOVER_BOARD_X, 0.220, HOVER_BOARD_Z, 0.0,  1.0,  0.0,  0.0);
    return goToPose(HOVER_BOARD_X, HOVER_BOARD_Y, HOVER_BOARD_Z, 1.0, -0.03, 0, 0);
}

bool TTTController::hoverAboveCenterOfBoard()
{
    ROS_DEBUG("Hovering above center of board..");
    bool res;

    if (_legacy_code == true)
    {
        return goToPoseNoCheck(HOVER_BOARD_X + _offsets[4].x,
                        HOVER_BOARD_Y + _offsets[4].y,
                        HOVER_BOARD_Z - _offsets[4].z + 0.3,    // TODO this minus sign is a bug
                        VERTICAL_ORI_L);
    }
    else
    {
        res = goToPose(_board_centers_poss[4].x,
                        _board_centers_poss[4].y,
                        _board_centers_poss[4].z + 0.3,
                        VERTICAL_ORI_L);
         ROS_INFO("Hover the centre of the board %s", res ? "Success":"Fail");
         return res;
    }
}

bool TTTController::hoverAboveCell(double height)
{
    ROS_DEBUG("Hovering above cell..");

    if (_legacy_code == true)
    {
        return goToPose(HOVER_BOARD_X + _offsets[getObjectID()-1].x,
                        HOVER_BOARD_Y + _offsets[getObjectID()-1].y,
                        HOVER_BOARD_Z - _offsets[getObjectID()-1].z + 0.05,
                        VERTICAL_ORI_L);
    }
    else
    {
    	ROS_INFO("Hover above cell %d at x:%f y:%f ",
    			getObjectID(),
    			_board_centers_poss[getObjectID()-1].x,
    			_board_centers_poss[getObjectID()-1].y);
        return goToPose(_board_centers_poss[getObjectID()-1].x,
                        _board_centers_poss[getObjectID()-1].y,
                        _board_centers_poss[getObjectID()-1].z + height,
                        VERTICAL_ORI_L);
    }
}

bool TTTController::hoverAboveTokens(double height)
{
	/*x: 0.556375635921
    y: -0.579*/
    return goToPose(0.599, -0.611, height+0.1, VERTICAL_ORI_L);
}

bool TTTController::scanBoardImpl()
{
    if (_legacy_code == false)
    {
        return true;
    }

    ROS_DEBUG("Scanning depth..");
    if (!hoverAboveBoard()) return false;

    // wait for image callback
    while(RobotInterface::ok())
    {
        if(!_is_img_empty) break;

        r.sleep();
    }

    float dist;
    setDepth(dist);
    if (!hoverAboveBoard()) return false;
    processImage(dist);

    ROS_DEBUG("Hovering above tokens..");
    hoverAboveTokens(Z_LOW);

    return true;
}

bool TTTController::pickUpTokenImpl()
{
	ros::Duration rest_time(1);
    ROS_INFO("Picking up token..");
    setTracIK(true);

    while(RobotInterface::ok())
    {
        if(isIRok()) break;
        r.sleep();
    }

    // wait for image callback
    while(RobotInterface::ok())
    {
        if(!_is_img_empty) break;
        r.sleep();
    }

    ROS_INFO("token pos: x = %f, y = %f", _board_centers_poss[getObjectID()-1].x,
    									  _board_centers_poss[getObjectID()-1].y); // Debug
    if(getObjectID() > 9){
    	hoverAboveTokens(Z_LOW); // playing

    }
    else {
    		hoverAboveTokens(Z_LOW);
        	hoverAboveCenterOfBoard(); // cleaning
        }// cleaning

    hoverAboveCell(0.03); // hover above the cell or pool containing the next tile
    rest_time.sleep();
    gripToken();
    hoverAboveCell(0.1); // lift tile up

    if(getObjectID() > 9) hoverAboveTokens(Z_LOW); // playing
    else {
    	//hoverAboveTokens(Z_LOW);
    	hoverAboveCenterOfBoard(); // cleaning
    }
    setTracIK(false);

    return true;
}

bool TTTController::putDownTokenImpl()
{
    ROS_DEBUG("Putting down token..");
    if(getObjectID() < 9)
    	{if (!hoverAboveCenterOfBoard())
    	{
    		ROS_WARN("Fail to hover over Centre of board.");
    		return false;
    	}
    }
    else {if (!hoverAboveTokens(Z_LOW)) return false;}
    if (!hoverAboveCell(0.00))
    {
    	ROS_WARN("Fail to hover over cell %d", getObjectID());
    	return false;
    }
    ros::Duration(0.1).sleep();
    if (!releaseObject()) return false;
    if(getObjectID() < 9) {if (!hoverAboveCenterOfBoard()) return false;}
    //else {if (!hoverAboveTokens(Z_LOW)) return false;}
    hoverAboveTokens(Z_LOW);
    //goHome();
    return true;
}

void TTTController::imageCb(const sensor_msgs::ImageConstPtr& msg)
{
    cv_bridge::CvImageConstPtr cv_ptr;

    try
    {
        cv_ptr = cv_bridge::toCvShare(msg);
    }
    catch(cv_bridge::Exception& e)
    {
        ROS_ERROR("[TTT Controller] cv_bridge exception: %s", e.what());
        return;
    }

    pthread_mutex_lock(&_mutex_img);
    _curr_img     = cv_ptr->image.clone();
    _img_size     =      _curr_img.size();
    _is_img_empty =     _curr_img.empty();
    imshow("Hand Camera", _curr_img.clone());
    pthread_mutex_unlock(&_mutex_img);
}

TTTController::~TTTController()
{
    destroyCVWindows();

    pthread_mutex_destroy(&_mutex_img);
}


//------------------------------------------------------- METHODS ------------------------------------------------------


/*
 * Constructor : only initialises all fields;
 */

template <class MotorClass, class CoordinateSystem>
MatrixDrone<MotorClass, CoordinateSystem>::MatrixDrone() : motors_locked(false), powerMatrix(new Matrix()), nbMotors(0), motors(nullptr),
                                                           nbCoordinates(0), initFlag(false) {}


/*
 * Destructor : frees the motor array, and deletes the matrix;
 */

template <class MotorClass, class CoordinateSystem>
MatrixDrone<MotorClass, CoordinateSystem>::~MatrixDrone() {

    //Free the motors arrays;
    deleteMotorsArray();

    delete powerMatrix;

}


/*
 * deleteMotorsArray : this function frees the entire motors array, and resets its size to zero;
 */

template <class MotorClass, class CoordinateSystem>
void MatrixDrone<MotorClass, CoordinateSystem>::deleteMotorsArray() {

    //First, free all motors.

    //For each motor data :
    for (uint8_t motor_index = 0; motor_index < nbMotors; motor_index++) {

        //Delete the motor data;
        delete motors[motor_index];

    }

    //Now, delete the pointer array;
    free(motors);

    //Nullify the single motors array;
    motors = nullptr;

    //Reset the number of single motors;
    nbMotors = 0;

}


/*
 * failSafe : this function will reset properly all processing data after an init failure;
 */

template <class MotorClass, class CoordinateSystem>
void MatrixDrone<MotorClass, CoordinateSystem>::failSafe() {

    //First, delete the power matrix for safety;
    delete powerMatrix;

    //Reset the number of coordinates to zero;
    nbCoordinates = 0;

    //Reset the motors array;
    deleteMotorsArray();

    //Reset the init flag;
    initFlag = false;

}



/*
 * addMotor : this function adds a motor to the motors array.
 *
 *  It starts by adding one space to the array using realloc, and then copies the content of the motor data
 *      in this new space;
 */

template <class MotorClass, class CoordinateSystem>
void MatrixDrone<MotorClass, CoordinateSystem>::addMotor(MotorClass *motor_data) {

    //If the geometry is locked, fail;
    if (motors_locked) {

        //Log
        std_out("Error in MatrixDrone<MotorClass, CoordinateSystem>::setRegistered : the geometry is locked, no motors can be added anymore.");

        //Return the default index;
        return;

    }

    //First, increment the number of single motors;
    uint8_t new_index = nbMotors++;

    //Try to reallocate the array;
    void *new_ptr = realloc(motors, nbMotors * sizeof(MotorClass *));

    //If the reallocation failed :
    if (!new_ptr) {

        //Restore the number of single motors;
        nbMotors = new_index;

        //Log
        std_out("Error in MatrixDrone<MotorClass, CoordinateSystem>::setRegistered : the reallocation failed.");

        //Fail with default value;
        return;

    }

    //Update the motor array
    motors = (MotorClass **) (new_ptr);

    //Copy the new motor data at the new position;
    motors[new_index] = motor_data;


}


/*
 * solve : this function will solve the drone's equation system.
 */
template <class MotorClass, class CoordinateSystem>
void MatrixDrone<MotorClass, CoordinateSystem>::solve() {

    /*
    * Init;
    */

    //Create a coordinate system struct;
    CoordinateSystem coordinate_system = CoordinateSystem();

    //First, let the sub-class declare its coordinate system;
    setCoordinateSystem(&coordinate_system);

    //Then, let the sub-class create all motors;
    createMotors();

    //Provide any other motor to be added;
    motors_locked = true;


    //Verify that enough motors have been added
    const uint8_t nb_coordinates = getCoordinatesNumber(&coordinate_system);

    //If there are not enough motors, fail
    if (nbMotors < nb_coordinates) {

        //Log;
        std_out("Error in MatrixDrone<MotorClass, CoordinateSystem>::MatrixDrone : You didn't provide a sufficient number of motor to "
                        "be able to control all your coordinates.");


        //Fail safely;
        failSafe();

        //Stop;
        return;

    }


    /*
     * System resolution;
     */

    //TODO

    //Now, add the motors equations to the system;
    //addMotorsEquations(&coordinate_system, system);

    //Then, let the sub-class add its own relations;
    //createRelations(system);

    //Now, extract the control matrix, for the threshold check;
    const Matrix *control_matrix = new Matrix();//TODO system->extractMatrix();


    /*
     * Threshold control;
     */

    //Then, check if some dimensions can't be controlled;
    bool drone_controllable = checkControl(control_matrix, &coordinate_system);

    //The control matrix is not used after this, so we will delete it;
    delete control_matrix;

    //If one coordinate is not controllable, fail;
    if (!drone_controllable) {

        //Delete the equation system;
        delete system;

        //Log;
        std_out("Error in MatrixDrone<MotorClass, CoordinateSystem>::MatrixDrone : One line in the equation controlMatrix is below the "
                        "provided threshold.");

        //Fail safely;
        failSafe();

        //Stop;
        return;

    }



    /*
     * System resolution;
     */

    //Create the linear system that will represent our physical model;
    LinearSystem *system = new LinearSystem(nbMotors);


    //Solve the system, and get the control matrix;
    //powerMatrix = system->solve();

    //Delete the system;
    delete system;


    if (powerMatrix == nullptr) {

        //Log;
        std_out("Error in MatrixDrone<MotorClass, CoordinateSystem>::MatrixDrone : Failed to solve the system.");

        //Fail safely;
        failSafe();

        //Stop;
        return;

    }

    //Log;
    std_out("MatrixDrone instance properly initialised and ready.");

    //Finally, mark the initialisation as finished and properly executed;
    initFlag = true;

}


/*
 * infiniteNorm : this function calculates the infinite norm of a float vector;
 */

template <class MotorClass, class CoordinateSystem>
float MatrixDrone<MotorClass, CoordinateSystem>::infiniteNorm(const float *matrix_line, uint8_t size) {

    //As we want to determine the maximum of a positive set of float, solve a max_variable to zero;
    float max_value = 0;

    //Iterate [size] times;
    for (; size--;) {

        //Cache the value at the current index and increment the counter;
        float value = *(matrix_line++);

        //Take the absolute value;
        if (value < 0) value = -value;

        //Update maximum if required;
        if (max_value < value) max_value = value;

    }

    return max_value;

}


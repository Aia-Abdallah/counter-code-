/*counter code   By Aya.A */

//ROSS serial libirary


#include <ros.h>  // header files for any messages that you will be using in ROS
#include <std_msgs/Float32.h> //(Float32)type of data that while publish
ros::NodeHandle nh; //The node uses a ros::NodeHandle object to connect to ROS.

/*
 *Define publishers for each value you want to publish:


*/
//std_msgs::Float32 pulseCountMsg; 
std_msgs::Float32 displacementMsg;
std_msgs::Float32 velocityMsg;
std_msgs::Float32 accelerationMsg;
/*
*using <ros::Publisher> objects to active topic
*/
//ros::Publisher pulseCountPublisher("pulse_count", &pulseCountMsg);
ros::Publisher displacementPublisher("counter_node/displacement", &displacementMsg);
ros::Publisher velocityPublisher("counter_node/velocity",&velocityMsg);
ros::Publisher accelerationPublisher("counter_node/acceleration", &accelerationMsg);


//** Pin configuration & variables**//
/*************************************************************************
 *int0Pin -> Assigns pin 3 as the input pin for external interrupts (INT0).
 *startTime -> to store time
 *displacement -> to store displacement
 *accelaration -> to store accelaration
 *pulseCount -> to store pulse count & volatile (to ensure value always up-to-date in memory)
***************************************************************************/
const int int0Pin = 3; 
unsigned long startTime = 0;
float displacement =0;
float velocity =0;
float acceleration =0;
volatile int pulseCount = 0;

// Interrupt Service Routine (ISR) for INT0
void int0ISR() {
  pulseCount++;
}


/***************************************************************************
 //function to calculate Displacement
 *diamiter for wheel = 50 cm 
 *circumference for wheel = 2pi*R = 1.5 m
 * 52.6 pules for one rev
 *pulse factor = C/pulses per revelution = 0.028517

***************************************************************************/
void calculateDisplacement(){
  float pulseFactor = 0.028517;
  displacement = pulseCount * pulseFactor;
}
/***************************************************************************
 //function to calculate velocity
 * used millis() function for time 
 1-Because it retern time in millsecond
 2-non-blocking (no interrupt tje flow code)

***************************************************************************/
void calculateVelocity(){
  unsigned long currentTime = millis();
  float timeElapsed = (currentTime -startTime) /1000 ;//to converted to second
  velocity = displacement * timeElapsed;
}
/*************************************************************************
 //function to calculat accelaration
 * acc = velocity /time
***************************************************************************/
void calculateAcceleration(){
  unsigned long currentTime = millis();
  float timeElapsed = (currentTime -startTime) /1000 ;//to converted to second
  acceleration = velocity / timeElapsed ;
}
/*************************************************************************
  //FUnction to Initialize serial communication
  *The attachInterrupt function is used with the RISING mode
**************************************************************************/
void setup() {

  nh.initNode("counter_node"); //<counter_node> that is the name node of publisher
  nh.advertise(pulseCountPublisher);
  nh.advertise(displacementPublisher);
  nh.advertise(velocityPublisher);
  nh.advertise(accelerationPublisher); 

  Serial.begin(9600); //baude rate
  startTime =millis();  //record the initial time 
  pinMode(int0Pin, INPUT); // Configure INT0 pin as input

  // Enable external interrupt INT0
   attachInterrupt(digitalPinToInterrupt(int0Pin), int0ISR, RISING);
}

void loop() {
  pulseCountMsg.data = pulseCount;
  displacementMsg.data = displacement;
  velocityMsg.data = velocity;
  accelerationMsg.data = acceleration;

  pulseCountPublisher.publish(&pulseCountMsg);
  displacementPublisher.publish(&displacementMsg);
  velocityPublisher.publish(&velocityMsg);
  accelerationPublisher.publish(&accelerationMsg);

  nh.spinOnce();





   calculateDisplacement();
   calculateVelocity();
   calculateAcceleration();
   Serial.print("Pulse Count: " + String(pulseCount)); 
   Serial.print("Displacement: " + String(displacement));
   Serial.print("Velocity: " + String(velocity));
   Serial.print("Acceleration: " + String(acceleration));
 
 
   delay(10); // Delay to make the output readable


 }

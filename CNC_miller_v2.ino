#include <MapleFreeRTOS821.h>
#include "Stepper.h"
#include "SimpleList.h"

#define STEPS 2048

bool crash_flag = false;

//pin for axises with collision
int x_switch_1 = 24, x_switch_2 = 25;
int y_switch_1 = 26, y_switch_2 = 27;
int z_switch_1 = 28, z_switch_2 = 29;

//current position of tool
double current_x = 0.0;
double current_y = 0.0;
double current_z = 0.0;
//parameters for stepper
double StepsPerMillimeterX = 683.0;
double StepsPerMillimeterY = 683.0;
double StepsPerMillimeterZ = 683.0;
long x_itinerary = 0;
long y_itinerary = 0;
long z_itinerary = 0;

//the matrix to present the platform
float bed_level_matrix[9] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

Stepper stepper_x(STEPS, 8, 10, 9, 11);
Stepper stepper_y(STEPS, 12, 14, 13, 15);
Stepper stepper_z(STEPS, 16, 18, 17, 19);
Stepper stepper_tool(STEPS, 20, 22, 21, 23);

static void readCommand(void *pvParameters)
{
    for (;;)
    {
        if(Serial.available()>0)
        {
          String message = Serial.readString();
          Serial.println("gcode: " + message);

          if(command_processor(stepper_x, stepper_y, stepper_z, stepper_tool, message))
          {
            Serial.print("execute at line:");
            Serial.println(message);
          }
          else
          {
            crash_flag = true;
            Serial.print("crash at line:");
            Serial.println(message);
          }
          vTaskDelay(1000);
         }   
    }
}

static void moveXaxis(void *pvParameters)
{
    for (;;)
    {
        vTaskDelay(1000);
    }
}

static void moveYaxis(void *pvParameters)
{
    for (;;)
    {
        vTaskDelay(1000);
    }
}

static void moveZaxis(void *pvParameters)
{
    for (;;)
    {
        vTaskDelay(1000);
    }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(1000);

  init_machine(stepper_x, stepper_y, stepper_z, stepper_tool, x_switch_1, x_switch_2, y_switch_1, y_switch_2, z_switch_1, z_switch_2);
  
  xTaskCreate(readCommand,
                "readCommandTask",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 2,
                NULL);

  xTaskCreate(moveXaxis,
                "readCommandTask",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 2,
                NULL);

  xTaskCreate(moveYaxis,
                "readCommandTask",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 2,
                NULL);

  xTaskCreate(moveZaxis,
                "readCommandTask",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 2,
                NULL);
  vTaskStartScheduler();
  
  Serial.println("machine power on");
  Serial.println();

}

void loop() {
    // Insert background code here
}

void init_machine(Stepper &stepper_x, Stepper &stepper_y, Stepper &stepper_z, Stepper &stepper_tool, int x_sw1, int x_sw2, int y_sw1, int y_sw2, int z_sw1, int z_sw2)
{
  // set the speed of the motor to 30 RPMs
  stepper_x.setSpeed(5);
  stepper_y.setSpeed(5);
  stepper_z.setSpeed(5);
  stepper_tool.setSpeed(5);

  pinMode(x_sw1, INPUT_PULLUP);
  pinMode(x_sw2, INPUT_PULLUP);
  pinMode(y_sw1, INPUT_PULLUP);
  pinMode(y_sw2, INPUT_PULLUP);
  pinMode(z_sw1, INPUT_PULLUP);
  pinMode(z_sw2, INPUT_PULLUP);
}

bool command_processor(Stepper &stepper_x, Stepper &stepper_y, Stepper &stepper_z, Stepper &stepper_tool, String command)
{
  int commaPosition;
  SimpleList<String> *mySimpleList = new SimpleList<String>();
  bool status_flag = true;

  //spilt the commands
  do{
      commaPosition = command.indexOf(' ');
      if(commaPosition != -1)
      {
         Serial.println(command.substring(0,commaPosition));
         mySimpleList->add(command.substring(0,commaPosition));
         command=command.substring(commaPosition+1,command.length());
      }
      else{
        if(command.length()>0){
           Serial.println(command);
        }
      }
   }
   while(commaPosition>=0);
   mySimpleList->add(command);

   //build the axis table(the command used which axis)
   unsigned int table = 0b0000000000000000000000000000000;
   String current_field;
   for(int i = 0; i < mySimpleList->size(); i++)
   {
      current_field = mySimpleList->get(i);
      if(current_field.charAt(0) == 'G')
      {
        table = table || 0b0000000000000000000000000000001;
      }
      else if(current_field.charAt(0) == 'M')
      {
        table = table || 0b0000000000000000000000000000010;
      }
      else if(current_field.charAt(0) == 'T')
      {
        table = table || 0b0000000000000000000000000000100;
      }
      else if(current_field.charAt(0) == 'S')
      {
        table = table || 0b0000000000000000000000000001000;
      }
      else if(current_field.charAt(0) == 'P')
      {
        table = table || 0b0000000000000000000000000010000;
      }
      else if(current_field.charAt(0) == 'X')
      {
        table = table || 0b0000000000000000000000000100000;
      }
      else if(current_field.charAt(0) == 'Y')
      {
        table = table || 0b0000000000000000000000001000000;
      }
      else if(current_field.charAt(0) == 'Z')
      {
        table = table || 0b0000000000000000000000010000000;
      }
      else if(current_field.charAt(0) == 'U')
      {
        table = table || 0b0000000000000000000000100000000;
      }
      else if(current_field.charAt(0) == 'V')
      {
        table = table || 0b0000000000000000000001000000000;
      }
      else if(current_field.charAt(0) == 'W')
      {
        table = table || 0b0000000000000000000010000000000;
      }
      else if(current_field.charAt(0) == 'I')
      {
        table = table || 0b0000000000000000000100000000000;
      }
      else if(current_field.charAt(0) == 'J')
      {
        table = table || 0b0000000000000000001000000000000;
      }
      else if(current_field.charAt(0) == 'D')
      {
        table = table || 0b0000000000000000010000000000000;
      }
      else if(current_field.charAt(0) == 'H')
      {
        table = table || 0b0000000000000000100000000000000;
      }
      else if(current_field.charAt(0) == 'F')
      {
        table = table || 0b0000000000000001000000000000000;
      }
      else if(current_field.charAt(0) == 'R')
      {
        table = table || 0b0000000000000010000000000000000;
      }
      else if(current_field.charAt(0) == 'Q')
      {
        table = table || 0b0000000000000100000000000000000;
      }
      else if(current_field.charAt(0) == 'E')
      {
        table = table || 0b0000000000001000000000000000000;
      }
      else if(current_field.charAt(0) == 'N')
      {
        table = table || 0b0000000000010000000000000000000;
      }
      else if(current_field.charAt(0) == '*')
      {
        table = table || 0b0000000000100000000000000000000;
      }
      else
      {
        
      }
   }

   /*//branch with gcode command
   current_field = mySimpleList->get(0);
   if(current_field == "G0")
   {
    status_flag = G0(stepper_x, stepper_y, stepper_z, stepper_tool, table, mySimpleList);
   }
   else if(current_field == "G1")
   {
    status_flag = G1(stepper_x, stepper_y, stepper_z, stepper_tool, table, mySimpleList);
   }
   else if(current_field == "G2")
   {
    status_flag = G2(stepper_x, stepper_y, stepper_z, stepper_tool, table, mySimpleList);
   }
   else if(current_field == "G3")
   {
    status_flag = G3(stepper_x, stepper_y, stepper_z, stepper_tool, table, mySimpleList);
   }
   else if(current_field == "G28")
   {
    status_flag = G28(stepper_x, stepper_y, stepper_z, stepper_tool);
   }
   else if(current_field == "G29")
   {
    status_flag = G29(stepper_x, stepper_y, stepper_z, stepper_tool);
   }
   else
   {
    
   }*/

   //if execute success then return true
   return status_flag;
}

bool collision_detect()
{
  bool collision = false;
  for(int i = 24; i < 30;i++)
  {
    /*if(digitalRead(i))
    {
      collision = !collision;
      break;
    }*/
  }

  return collision;
}

bool G0(Stepper &stepper_x, Stepper &stepper_y, Stepper &stepper_z, Stepper &stepper_tool, unsigned int axis_table, SimpleList<String> *fields)
{
  bool x_enable = false, y_enable = false, z_enable = false, e_enable = false, f_enable = false, s_enable = false;
  
  x_enable = axis_table && 0b0000000000000000000000000100000;
  y_enable = axis_table && 0b0000000000000000000000001000000;
  z_enable = axis_table && 0b0000000000000000000000010000000;
  e_enable = axis_table && 0b0000000000001000000000000000000;
  f_enable = axis_table && 0b0000000000000001000000000000000;
  s_enable = axis_table && 0b0000000000000000000000000001000;

  double x_para = 0.0, y_para = 0.0, z_para = 0.0, e_para = 0.0, f_para = 1.0, s_para = 0.0;

  for(int i = 1; i < fields->size(); i++)
  {
    Serial.print("head:");Serial.println(fields->get(i).charAt(0));
    if(fields->get(i).charAt(0) == 'X' && x_enable)
    {
      Serial.println(atof(fields->get(i).substring(1,fields->get(i).length()).c_str()));
      x_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
    else if(fields->get(i).charAt(0) == 'Y' && y_enable)
    {
      Serial.println(atof(fields->get(i).substring(1,fields->get(i).length()).c_str()));
      y_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
    else if(fields->get(i).charAt(0) == 'Z' && z_enable)
    {
      Serial.println(atof(fields->get(i).substring(1,fields->get(i).length()).c_str()));
      z_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
    else if(fields->get(i).charAt(0) == 'E' && e_enable)
    {
      e_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
    else if(fields->get(i).charAt(0) == 'S' && s_enable)
    {
      s_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
    else if(fields->get(i).charAt(0) == 'F' && f_enable)
    {
      f_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
  }

  Serial.print("x_enable:");Serial.println(x_enable);
  Serial.print("y_enable:");Serial.println(y_enable);
  Serial.print("z_enable:");Serial.println(z_enable);
  Serial.print("x_para:");Serial.println(x_para);
  Serial.print("y_para:");Serial.println(y_para);
  Serial.print("z_para:");Serial.println(z_para);

  double dx = 0.0;
  double dy = 0.0;
  double dz = 0.0;
  
  if(x_enable)
  {
    dx = pow(abs(x_para-current_x), 2);
  }
  if(y_enable)
  {
    dy = pow(abs(y_para-current_y), 2);
  }
  if(z_enable)
  {
    dz = pow(abs(z_para-current_z), 2);
  }
  
  //double distance = sqrt(pow(abs(x_para - current_x), 2) + pow(abs(y_para - current_y), 2) + pow(abs(z_para - current_z), 2));
  double distance = sqrt(dx + dy +dz);

  int loops = 0;
  if(f_enable)
  {
    //int loops = (int)(distance / f_enable) + 1;
    loops = (int)(distance / (f_para)) + 1;
  }
  else
  {
    loops = (int)(distance / (f_para)) + 1;
  }

  Serial.print("loops:");Serial.println(loops);
  
  if(x_enable)
  {
    dx = (x_para-current_x)/(double)(loops) * StepsPerMillimeterX;
  }
  if(y_enable)
  {
    dy = (y_para-current_y)/(double)(loops) * StepsPerMillimeterY;
  }
  if(z_enable)
  {
    dz = (z_para-current_z)/(double)(loops) * StepsPerMillimeterZ;
  }

  Serial.print("dx:");Serial.println(dx);
  Serial.print("dy:");Serial.println(dy);
  Serial.print("dz:");Serial.println(dz);

  for(int i = 0; i < loops; i++)
  {
    if(x_enable)
    {
      stepper_x.step((int)dx);
    }
    if(y_enable)
    {
      stepper_y.step((int)dy);
    }
    if(z_enable)
    {
      stepper_z.step((int)dz);
    }

    delay(100);

    if(collision_detect())
    {
      return false;
    }
  }

  if(x_enable)
  {
    current_x = x_para;
  }
  if(y_enable)
  {
    current_y = y_para;
  }
  if(z_enable)
  {
    current_z = z_para;
  }
  x_enable = false, y_enable = false, z_enable = false, e_enable = false, f_enable = false, s_enable = false;
  x_para = 0.0, y_para = 0.0, z_para = 0.0, e_para = 0.0, f_para = 10.0, s_para = 0.0;

  return true;
}

bool G1(Stepper &stepper_x, Stepper &stepper_y, Stepper &stepper_z, Stepper &stepper_tool, unsigned int axis_table, SimpleList<String> *fields)
{
  bool x_enable = false, y_enable = false, z_enable = false, e_enable = false, f_enable = false, s_enable = false;
  
  x_enable = axis_table && 0b0000000000000000000000000100000;
  y_enable = axis_table && 0b0000000000000000000000001000000;
  z_enable = axis_table && 0b0000000000000000000000010000000;
  e_enable = axis_table && 0b0000000000001000000000000000000;
  f_enable = axis_table && 0b0000000000000001000000000000000;
  s_enable = axis_table && 0b0000000000000000000000000001000;

  double x_para = 0.0, y_para = 0.0, z_para = 0.0, e_para = 0.0, f_para = 10.0, s_para = 0.0;

  for(int i = 1; i < fields->size(); i++)
  {
    Serial.print("head:");Serial.println(fields->get(i).charAt(0));
    if(fields->get(i).charAt(0) == 'X' && x_enable)
    {
      Serial.println(atof(fields->get(i).substring(1,fields->get(i).length()).c_str()));
      x_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
    else if(fields->get(i).charAt(0) == 'Y' && y_enable)
    {
      Serial.println(atof(fields->get(i).substring(1,fields->get(i).length()).c_str()));
      y_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
    else if(fields->get(i).charAt(0) == 'Z' && z_enable)
    {
      Serial.println(atof(fields->get(i).substring(1,fields->get(i).length()).c_str()));
      z_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
    else if(fields->get(i).charAt(0) == 'E' && e_enable)
    {
      e_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
    else if(fields->get(i).charAt(0) == 'S' && s_enable)
    {
      s_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
    else if(fields->get(i).charAt(0) == 'F' && f_enable)
    {
      f_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
  }

  Serial.print("x_enable:");Serial.println(x_enable);
  Serial.print("y_enable:");Serial.println(y_enable);
  Serial.print("z_enable:");Serial.println(z_enable);
  Serial.print("x_para:");Serial.println(x_para);
  Serial.print("y_para:");Serial.println(y_para);
  Serial.print("z_para:");Serial.println(z_para);

  double dx = 0.0;
  double dy = 0.0;
  double dz = 0.0;
  
  if(x_enable)
  {
    dx = pow(abs(x_para-current_x), 2);
  }
  if(y_enable)
  {
    dy = pow(abs(y_para-current_y), 2);
  }
  if(z_enable)
  {
    dz = pow(abs(z_para-current_z), 2);
  }
  
  //double distance = sqrt(pow(abs(x_para - current_x), 2) + pow(abs(y_para - current_y), 2) + pow(abs(z_para - current_z), 2));
  double distance = sqrt(dx + dy +dz);

  int loops = 0;
  if(f_enable)
  {
    //int loops = (int)(distance / f_enable) + 1;
    loops = (int)(distance / (f_para)) + 1;
  }
  else
  {
    loops = (int)(distance / (f_para)) + 1;
  }

  Serial.print("loops:");Serial.println(loops);
  
  if(x_enable)
  {
    dx = (x_para-current_x)/(double)(loops) * StepsPerMillimeterX;
  }
  if(y_enable)
  {
    dy = (y_para-current_y)/(double)(loops) * StepsPerMillimeterY;
  }
  if(z_enable)
  {
    dz = (z_para-current_z)/(double)(loops) * StepsPerMillimeterZ;
  }

  Serial.print("dx:");Serial.println(dx);
  Serial.print("dy:");Serial.println(dy);
  Serial.print("dz:");Serial.println(dz);

  for(int i = 0; i < loops; i++)
  {
    if(x_enable)
    {
      stepper_x.step(dx);
    }
    if(y_enable)
    {
      stepper_y.step(dy);
    }
    if(z_enable)
    {
      stepper_z.step(dz);
    }

    delay(500);

    if(collision_detect())
    {
      return false;
    }
  }

  if(x_enable)
  {
    current_x = x_para;
  }
  if(y_enable)
  {
    current_y = y_para;
  }
  if(z_enable)
  {
    current_z = z_para;
  }
  x_enable = false, y_enable = false, z_enable = false, e_enable = false, f_enable = false, s_enable = false;
  x_para = 0.0, y_para = 0.0, z_para = 0.0, e_para = 0.0, f_para = 10.0, s_para = 0.0;

  return true;
}

bool G2(Stepper &stepper_x, Stepper &stepper_y, Stepper &stepper_z, Stepper &stepper_tool, unsigned int axis_table, SimpleList<String> *fields)
{
  bool x_enable = false, y_enable = false, i_enable = false, j_enable = false, e_enable = false, f_enable = false;
  x_enable = axis_table && 0b0000000000000000000000000100000;
  y_enable = axis_table && 0b0000000000000000000000001000000;
  i_enable = axis_table && 0b0000000000000000000100000000000;
  j_enable = axis_table && 0b0000000000000000001000000000000;
  e_enable = axis_table && 0b0000000000001000000000000000000;
  f_enable = axis_table && 0b0000000000000001000000000000000;

  double x_para = 0.0, y_para = 0.0, i_para = 0.0, j_para = 0.0, e_para = 0.0, f_para = 0.0;

  for(int i = 0; i < fields->size(); i++)
  {
    if(fields->get(i).charAt(0) == 'X' && x_enable)
    {
      x_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
    if(fields->get(i).charAt(0) == 'Y' && y_enable)
    {
      y_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
    if(fields->get(i).charAt(0) == 'I' && i_enable)
    {
      i_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
    if(fields->get(i).charAt(0) == 'J' && j_enable)
    {
      j_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
    if(fields->get(i).charAt(0) == 'E' && e_enable)
    {
      e_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
    if(fields->get(i).charAt(0) == 'F' && f_enable)
    {
      f_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
  }

  double dx = 0.0;
  double dy = 0.0;
  double distance = 0.0;
  double center_x = 0.0, center_y = 0.0;
  double rad = 0.0;

  center_x = current_x - i_para;
  center_y = current_y - j_para;

  dx = -i_para;
  dy = -j_para;
  double vector1[2] = {dx, dy};
  dx = x_para-center_x;
  dy = y_para-center_y;
  double vector2[2] = {dx, dy};

  rad = (vector1[0] * vector2[0] + vector1[1] * vector2[1]) / (sqrt(pow(vector1[0],2) + pow(vector1[1],2)) * sqrt(pow(vector2[0],2) + pow(vector2[1],2)));
  rad = acos(rad);

  double base = (vector1[0] * 1.0 + vector1[1] * 0.0) / (sqrt(pow(vector1[0],2) + pow(vector1[1],2)) * 1.0);
  base = acos(base);

  distance = sqrt(pow(vector1[0],2)) * rad;
  
  int loops = 0;
  if(f_enable)
  {
    //int loops = (int)(distance / f_enable) + 1;
    loops = (int)(distance / (f_para / 60)) + 1;
  }
  else
  {
    loops = (int)(distance);
  }
  
  for(int i = 0; i < loops; i++)
  {
    //x' = cos(θ) * x - sin(θ) * y
    //y' = sin(θ) * x + cos(θ) * y
    dx = cos(base-rad*i/loops) * i_para - sin(base-rad*i/loops) * j_para;
    dy = sin(base-rad*i/loops) * i_para + cos(base-rad*i/loops) * j_para;

    dx = dx + center_x;
    dy = dy + center_y;

    //dx = (x_para-current_x)/(double)(loops) * StepsPerMillimeterX;
    dx = (dx - current_x) * StepsPerMillimeterX;
    dy = (dy - current_y) * StepsPerMillimeterY;
    stepper_x.step(dx);
    stepper_y.step(dy);
    delay(500);

    if(collision_detect())
    {
      return false;
    }
  }

  current_x = x_para;
  current_y = y_para;

  return true;
}

bool G3(Stepper &stepper_x, Stepper &stepper_y, Stepper &stepper_z, Stepper &stepper_tool, unsigned int axis_table, SimpleList<String> *fields)
{
  bool x_enable = false, y_enable = false, i_enable = false, j_enable = false, e_enable = false, f_enable = false;
  x_enable = axis_table && 0b0000000000000000000000000100000;
  y_enable = axis_table && 0b0000000000000000000000001000000;
  i_enable = axis_table && 0b0000000000000000000100000000000;
  j_enable = axis_table && 0b0000000000000000001000000000000;
  e_enable = axis_table && 0b0000000000001000000000000000000;
  f_enable = axis_table && 0b0000000000000001000000000000000;

  double x_para = 0.0, y_para = 0.0, i_para = 0.0, j_para = 0.0, e_para = 0.0, f_para = 0.0;

  for(int i = 0; i < fields->size(); i++)
  {
    if(fields->get(i).charAt(0) == 'X' && x_enable)
    {
      x_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
    if(fields->get(i).charAt(0) == 'Y' && y_enable)
    {
      y_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
    if(fields->get(i).charAt(0) == 'I' && i_enable)
    {
      i_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
    if(fields->get(i).charAt(0) == 'J' && j_enable)
    {
      j_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
    if(fields->get(i).charAt(0) == 'E' && e_enable)
    {
      e_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
    if(fields->get(i).charAt(0) == 'F' && f_enable)
    {
      f_para = atof(fields->get(i).substring(1,fields->get(i).length()).c_str());
    }
  }

  double dx = 0.0;
  double dy = 0.0;
  double distance = 0.0;
  double center_x = 0.0, center_y = 0.0;
  double rad = 0.0;

  center_x = current_x - i_para;
  center_y = current_y - j_para;

  dx = -i_para;
  dy = -j_para;
  double vector1[2] = {dx, dy};
  dx = x_para-center_x;
  dy = y_para-center_y;
  double vector2[2] = {dx, dy};

  rad = (vector1[0] * vector2[0] + vector1[1] * vector2[1]) / (sqrt(pow(vector1[0],2) + pow(vector1[1],2)) * sqrt(pow(vector2[0],2) + pow(vector2[1],2)));
  rad = acos(rad);

  double base = (vector1[0] * 1.0 + vector1[1] * 0.0) / (sqrt(pow(vector1[0],2) + pow(vector1[1],2)) * 1.0);
  base = acos(base);

  distance = sqrt(pow(vector1[0],2)) * rad;
  
  int loops = 0;
  if(f_enable)
  {
    //int loops = (int)(distance / f_enable) + 1;
    loops = (int)(distance / (f_para / 60)) + 1;
  }
  else
  {
    loops = (int)(distance);
  }

  for(int i = 0; i < loops; i++)
  {
    //x' = cos(θ) * x - sin(θ) * y
    //y' = sin(θ) * x + cos(θ) * y
    dx = cos(base+rad*i/loops) * i_para - sin(base+rad*i/loops) * j_para;
    dy = sin(base+rad*i/loops) * i_para + cos(base+rad*i/loops) * j_para;

    dx = dx + center_x;
    dy = dy + center_y;

    //dx = (x_para-current_x)/(double)(loops) * StepsPerMillimeterX;
    dx = (dx - current_x) * StepsPerMillimeterX;
    dy = (dy - current_y) * StepsPerMillimeterY;
    stepper_x.step(dx);
    stepper_y.step(dy);
    delay(500);

    if(collision_detect())
    {
      return false;
    }
  }

  current_x = x_para;
  current_y = y_para;

  return true;
}

bool G28(Stepper &stepper_x, Stepper &stepper_y, Stepper &stepper_z, Stepper &stepper_tool)
{
    while(digitalRead(24))
    {
      stepper_x.step(-1);
      delay(500);
    }
    while(digitalRead(25))
    {
      stepper_x.step(1);
      x_itinerary++;
      delay(500);
    }
    for(long i = 0; i < x_itinerary/2; i++)
    {
      stepper_x.step(-1);
      delay(500);
    }

    while(digitalRead(26))
    {
      stepper_y.step(-1);
      delay(500);
    }
    while(digitalRead(27))
    {
      stepper_y.step(1);
      y_itinerary++;
      delay(500);
    }
    for(long i = 0; i < y_itinerary/2; i++)
    {
      stepper_y.step(-1);
      delay(500);
    }

    while(digitalRead(28))
    {
      stepper_z.step(1);
      delay(500);
    }
    while(digitalRead(29))
    {
      stepper_z.step(-1);
      z_itinerary++;
      delay(500);
    }
    for(long i = 0; i < z_itinerary; i++)
    {
      stepper_z.step(1);
      delay(500);
    }

    return true;
}

bool G29(Stepper &stepper_x, Stepper &stepper_y, Stepper &stepper_z, Stepper &stepper_tool)
{
  int delta_z = 0;
  
  G28(stepper_x, stepper_y, stepper_z, stepper_tool);

  for(long i = 0; i < z_itinerary-50; i++)
  {
    stepper_z.step(1);
    delay(500);
  }

  //point 1
  stepper_x.step(400);
  stepper_y.step(400);
  while(digitalRead(29))
  {
      stepper_z.step(-1);
      delta_z++;
      delay(500);
  }
  bed_level_matrix[0] = z_itinerary - 50 + delta_z;
  stepper_z.step(-delta_z);

  //point 2
  delta_z = 0;
  stepper_y.step(-400);
  while(digitalRead(29))
  {
      stepper_z.step(-1);
      delta_z++;
      delay(500);
  }
  bed_level_matrix[1] = z_itinerary - 50 + delta_z;
  stepper_z.step(-delta_z);

  //point 3
  delta_z = 0;
  stepper_y.step(-400);
  while(digitalRead(29))
  {
      stepper_z.step(-1);
      delta_z++;
      delay(500);
  }
  bed_level_matrix[2] = z_itinerary - 50 + delta_z;
  stepper_z.step(-delta_z);

  //point 4
  delta_z = 0;
  stepper_x.step(-400);
  while(digitalRead(29))
  {
      stepper_z.step(-1);
      delta_z++;
      delay(500);
  }
  bed_level_matrix[3] = z_itinerary - 50 + delta_z;
  stepper_z.step(-delta_z);

  //point 5
  delta_z = 0;
  stepper_y.step(400);
  while(digitalRead(29))
  {
      stepper_z.step(-1);
      delta_z++;
      delay(500);
  }
  bed_level_matrix[4] = z_itinerary - 50 + delta_z;
  stepper_z.step(-delta_z);

  //point 6
  delta_z = 0;
  stepper_y.step(400);
  while(digitalRead(29))
  {
      stepper_z.step(-1);
      delta_z++;
      delay(500);
  }
  bed_level_matrix[5] = z_itinerary - 50 + delta_z;
  stepper_z.step(-delta_z);

  //point 7
  delta_z = 0;
  stepper_x.step(-400);
  while(digitalRead(29))
  {
      stepper_z.step(-1);
      delta_z++;
      delay(500);
  }
  bed_level_matrix[6] = z_itinerary - 50 + delta_z;
  stepper_z.step(-delta_z);

  //point 8
  delta_z = 0;
  stepper_y.step(-400);
  while(digitalRead(29))
  {
      stepper_z.step(-1);
      delta_z++;
      delay(500);
  }
  bed_level_matrix[7] = z_itinerary - 50 + delta_z;
  stepper_z.step(-delta_z);

  //point 9
  delta_z = 0;
  stepper_y.step(-400);
  while(digitalRead(29))
  {
      stepper_z.step(-1);
      delta_z++;
      delay(500);
  }
  bed_level_matrix[8] = z_itinerary - 50 + delta_z;
  stepper_z.step(-delta_z);
  
  return true;
}



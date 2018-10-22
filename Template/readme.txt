/**
  ******************************************************************************
  * @file    Template/readme.txt
  * @author  MCU SD 
  * @version V2.0.0
  * @date    15-Jan-2016 
  * @brief   Description of the Template example.
  ******************************************************************************
  */

  @brief
    This example is used as a template project that can be used as reference to 
  build any new firmware application for GD32F130xx/150xx or GD32F170xx/190xx 
  devices using the GD32F1x0 Standard Peripherals Library.

  @Directory contents
  
    - Template/systick.c           Systick configuration file
    - Template/systick.h           Systick configuration header file
    - Template/gd32f1x0_conf.h     Library Configuration file
    - Template/gd32f1x0_it.c       Interrupt handlers
    - Template/gd32f1x0_it.h       Interrupt handlers header file
    - Template/main.c              Main program

  @Hardware and Software environment

    - This example runs on GD32F130xx/150xx and GD32F170xx/GD32F190xx devices.
  
    - This example has been tested with Gigadevice GD32130/150xx-EVAL
      (GD32F130/150xx Devices) and GD32170/190xx-EVAL (GD32F170/190xx Devices) 
      evaluation boards.
      This example can be easily tailored to any other supported device and 
    development board.

  @How to use it ? 

    In order to make the program work, you must do the following:
    + EWARM
      - Open the Project.eww workspace 
      - Rebuild all files: Project->Rebuild all
      - Load project image: Project->Debug
      - Run program: Debug->Go(F5)
 
    + MDK-ARM
      - Open the Project.uvproj project
      - Rebuild all files: Project->Rebuild all target files
      - Load project image: Debug->Start/Stop Debug Session
      - Run program: Debug->Run (F5) 

/****************** (C) COPYRIGHT 2016 GIGADEVICE *********END OF FILE*********/
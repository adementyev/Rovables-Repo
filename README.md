#Rovables Project 

##Introduction
This repository has all the design files to assemble the robots. Rovables are miniature wearable robots that can move around the body. 
See the paper we published at UIST conference [here](http://www.artemdementyev.com/wp-content/uploads/2016/10/Rovables-Miniature-On-Body-Robots-as-Mobile-Wearables.pdf).  

##Content
electronics/ - contains the circuit boards design. They were all designed using Altium, so it might be required to see the original files

mechanical/ - This folder contains mechanical files, done in SolidWorks 2016. 

software/ - contains the software that runs on the devices. We used the Arduino platform, so the robots can be easily programmed.  


##Manufacturing and design
The design contains many 3D printed custom parts and custom circuit boards. Making the design will require at least a good resolution 3D printer (Formlabs or Eden260). Other printers might work also, but we haven't tried them. 

##Known Issues
The localization of the robot is not implemented in the current code. This version has high resolution wheel encoders and a powerful ARM processor, so the tracking will be better. 

##Aknowledgements
This project was a collaboration between MIT Media lab (Responsive Environments and Living Mobile) and Shape Lab at Stanford University

The poeple who worked on this project are:

Graduate students: Artem Dementyev, Hsin-Liu (Cindy) Kao, Inrak Choi, Deborah Ajilo, Maggie Xu. 

The faculty: Sean Follmer, Joe Paradiso, Chris Shmandth.

Undegraduate students: Lucas Santana, Karla-Sofia Zapata-Garcia, Cheyenne Hua, and Diana Lamaute

![Shapelab Logo](/images/shapelab.png)
![MediaLab Logo](/images/medialablogo.png)

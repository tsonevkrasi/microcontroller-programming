     //-------------Initialization-----------------------------------------------

              //set I/O

     trisa = 0;             //clear a
     trisb = 0;             //Clear tris b
     trisc = 0;             //set TRIS insted PORT avoid Read-modify-write effect errors

     ansela = 0;         //0 = Digital input buffer enabled ,1=disabled
     trisa = 0;          //output
     lata.b4=0;

     anselb = 0;
     trisb = 0;          //output
     
     anselc.b3 = 0;
     anselc.b4 = 0;
     anselc.b5 = 0;
     trisc.b3 = 0;      //outputs
     trisc.b4 = 0;
     trisc.b5 = 0;
     latc.b3 = 1;
     latc.b4 = 1;
     latc.b5 = 1;

              //set oscillator

     OSCCON.SCS1 = 1;      //SCS-System Clock Select , 1x = Internal oscillator block
     OSCCON.IRCF2 = 1;     //IRCF-Internal RC Oscillator Frequency Select ,111=HFINTOSC–(16 MHz)
     OSCCON.IRCF1 = 1;
     OSCCON.IRCF0 = 1;

              //set interrupts
                            //RCON: RESET CONTROL REGISTER
     RCON.IPEN = 1;         //IPEN Interrupt Priority Enable,1=Enable priority levels
     INTCON.GIE_GIEH = 1;   //GIEH-Global Interrupt Enable bit,->0 when int. start ,->1 when int. finish
                            //When IPEN=1,1=Enables all high priority interrupts
                            //Disables all interrupts including low priority
     INTCON.PEIE_GIEL = 1;  //PEIE/GIEL: Peripheral Interrupt Enable
                            //When IPEN=1,1=Enables all low priority interrupts
                            //Disables all interrupts including low priority
                            //The “return from interrupt” instruction, RETFIE, exits
                            //the interrupt routine and sets the GIE/GIEH bit (GIEH
                            //or GIEL if priority levels are used), which re-enables interrupts.
          //set RS485 chanel 1

     BAUDCON1.BRG16 = 0;    //8 bit baud rate generator
     TXSTA1.BRGH = 0;       //BRGH-High Baud Rate Select bit,0=low speed
     SPBRG1 = 25;           //baud rate gen. timer
                            //SYNC=0,BRGH=0,BRG16=0,16 MHz,9600 Baud/sec
     anselc.b6 = 0;
     anselc.b7 = 0;
     TRISc.b6 = 1;          //TX2->TRIS=1
     TRISc.b7 = 1;          //RX2->TRIS=1
                            //RCSTAX-receive status and contr. reg.
     RCSTA1.SPEN = 1;       //1=Serial port enabled(configures RX and TX pins as serial port pins)
     TXSTA1.SYNC = 0;       //0=Asynchronous mode
     TXSTA1.TXEN = 1;       //1=Transmit enabled
     //TXSTA1.TX9 = 1;        //9-bit Transmit Enable bit, 1 = Selects 9-bit transmission

     //RCSTA1.RX9 = 1;        //9-bit Receive Enable bit
     RCSTA1.CREN = 1;       //Continuous Receive Enable bit. 1 = Enables receiver
     //RCSTA1.ADDEN = 1;      //Address Detect Enable bit, Asynchronous mode 9-bit (RX9 = 1):
                            //1 = Enables address detection, enable interrupt and load the receive buffer when RSR<8> is set
                            //must be clear after address verification to receive data characters and set again at the end of pacet
     lata.b5 = 1;           //receive data enable

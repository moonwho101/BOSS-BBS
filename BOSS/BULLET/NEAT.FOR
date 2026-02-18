
10                         THE FORTRAN LANGUAGE
11                         --------------------

  The Game of Life, Programmed By Mark Longo ...


   This project was done for those who are curious about the FORTRAN LANGUAGE.


C **********************************************************************
C   The Game Of Life , By Mark Longo
C **********************************************************************


C **********************************************************************
C Variables Used In Mainline
C
C      PLAY:  A logical variable to determine if users wishes to
C             play the game.
C **********************************************************************



C **********************************************************************
C   MAIN LINE FOLLOWS:
C **********************************************************************

      LOGICAL PLAY
      PLAY=.TRUE.
      WHILE (PLAY) DO
         CALL TITLE(PLAY)
         IF (PLAY) THEN
            CALL GENERATION
         ENDIF
      ENDWHILE


      STOP
      END

C **********************************************************************

C **********************************************************************

      SUBROUTINE TITLE(PLAY)
C
C      This routine will determine if the user wishes to play the
C      game.
C
C      VARIABLES USED:
C
C
C      RESPOND:  Used to ask player if he/she wishes to play.
C      PLAY:  Logical variable passed back to main line. Used to
C             determine if the computer should continue with game.
C
C
C








      LOGICAL PLAY
      CHARACTER * 1 RESPOND

C     CLEAR SCREEN AND SHOW MENU

      CALL CLS
      CALL DISPLAY(6,6,
     1'                  ษอออออออออออออออออออออออออออออออป')
      CALL DISPLAY(7,6,
     1'                  บ        The Game Of Life       บ')
      CALL DISPLAY(8,6,
     1'                  บ         By: Mark Longo        บ')
      CALL DISPLAY(9,6,
     1'                  บ      (27-150 Assignment 3)    บ')
      CALL DISPLAY(10,6,
     1'                  ศอออออออออออออออออออออออออออออออผ')

C    Ask if user wishes to play.

      CALL DISPLAY(16,26,'Do You Wish To Play (Y/N):  ')

      READ 10,RESPOND
   10 FORMAT (A)

      IF (RESPOND .EQ. 'Y' .OR. RESPOND .EQ. 'y') THEN
         PLAY=.TRUE.
      ELSE
          PLAY=.FALSE.
      ENDIF


      RETURN
      END
C **********************************************************************

C **********************************************************************



        SUBROUTINE GENERATION

C **********************************************************************

C    This routine will generate the game board and display the generations

C    VARIABLES USED:
C
C
C     NOABORT:  Logical. Used to determine if this subroutine should end
C     OUTER:    Logical. Used to determine if inner loop should end.
C     ROWST:    Used to place an organism in a ROW in the 2-d array
C     COLST:    Used to place an organism in a COLUMN in the 2-d array
C     GEN:      Which generation is being shown
C     COUNT:    Keeps track of living organisms
C     A1:       First 2-d array
C     A2:       Second 2-d array




C    Define Variables.

      LOGICAL NOABORT,OUTER
      INTEGER ROWST,COLST,GEN,R,C,COUNT
      CHARACTER * 1 A1(12,12),A2(12,12),ACT
      DATA A1,A2/144*' ',144*' '/
      GEN=0
      OUTER=.TRUE.

C     Call other subroutines

C     Clear Screen

      CALL CLS
      CALL DSCREEN
      CALL INIT(A1,ROWST,COLST,COUNT)


      WHILE (OUTER  .AND. COUNT .GT. 0) DO


        NOABORT=.TRUE.

        GEN=GEN+1

        CALL DISPLAY(20,4,'                  ')
        CALL DISPLAY(20,4,'GENERATION # ')

        PRINT 80,GEN

        CALL DISPLAY(21,4,'                      ')
        CALL DISPLAY(21,4,'COUNT # ')

        PRINT 80,COUNT

        CALL DISPLAY(23,6,'                                    ')
        CALL DISPLAY(22,4,'Press <<N>> for NEXT GENERATION')

        CALL DISPLAY(23,4,'Press <<Q>> and RETURN for MAIN MENU:')

   80   FORMAT(I3,'                              ')


        READ 200,ACT

  200   FORMAT(A)
        IF (ACT .EQ. 'N' .OR. ACT .EQ. 'n')  THEN

            NOABORT=.TRUE.

        ELSE

            NOABORT=.FALSE.
            OUTER=.FALSE.

        ENDIF


          WHILE (NOABORT) DO


               CALL RULES(A1,A2,COUNT)
               DO 170 R=1,12
               DO 171 C=1,12
               A1(R,C)=' '
  171  CONTINUE
  170  CONTINUE
               DO 150 R=1,12
               DO 155 C=1,12

               A1(R,C)=A2(R,C)

               CALL DISPLAY(R+4,39+((C+2)*2),A1(R,C))



  155 CONTINUE
  150 CONTINUE

               DO 180 R=1,12
               DO 181 C=1,12

               A2(R,C)=' '

  181 CONTINUE
  180 CONTINUE

               NOABORT=.FALSE.


          ENDWHILE



      ENDWHILE



      RETURN
      END


C **********************************************************************

        SUBROUTINE DSCREEN
C **********************************************************************
C     Subroutine to display GAME BOARD


         CALL DISPLAY(2,40,
     1'ษอออออออออออออออออออออออออออออออป')
      CALL DISPLAY(3,40,
     1'บ    0 0 0 0 0 0 0 0 0 1 1 1    บ')
      CALL DISPLAY(4,40,
     1'บ    1 2 3 4 5 6 7 8 9 0 1 2    บ')
      CALL DISPLAY(5,40,
     1'บ 01                         01 บ')
      CALL DISPLAY(6,40,
     1'บ 02                         02 บ')
      CALL DISPLAY(7,40,
     1'บ 03                         03 บ')
      CALL DISPLAY(8,40,
     1'บ 04                         04 บ')
      CALL DISPLAY(9,40,
     1'บ 05                         05 บ')
      CALL DISPLAY(10,40,
     1'บ 06                         06 บ')
      CALL DISPLAY(11,40,
     1'บ 07                         07 บ')
      CALL DISPLAY(12,40,
     1'บ 08                         08 บ')
      CALL DISPLAY(13,40,
     1'บ 09                         09 บ')
      CALL DISPLAY(14,40,
     1'บ 10                         10 บ')
      CALL DISPLAY(15,40,
     1'บ 11                         11 บ')
      CALL DISPLAY(16,40,
     1'บ 12                         12 บ')
      CALL DISPLAY(17,40,
     1'บ    0 0 0 0 0 0 0 0 0 1 1 1    บ')
      CALL DISPLAY(18,40,
     1'บ    1 2 3 4 5 6 7 8 9 0 1 2    บ')
      CALL DISPLAY(19,40,
     1'ศอออออออออออออออออออออออออออออออผ')


      CALL DISPLAY(5,2,'T H E    G A M E    O F    L I F E')
      CALL DISPLAY(8,2,'    Programmed by Mark Longo')
      CALL DISPLAY(10,2,'      (27-150 Fall 1988)')


      RETURN
      END

C **********************************************************************
C **********************************************************************

      SUBROUTINE INIT(X1,RW1,CL1,CN1)
C **********************************************************************

C   Subroutine to SET GAME BOARD

C    VARIABLES USED
C
C     X1:  The first Array
C     RW1: The row in the 2-d array
C     CL1: The column in the 2-d array
C     FLAGME:  Logical variable to see if users is finished
C     Check: Used in leaving subroutine





      CHARACTER * 1 X1(12,12)
      INTEGER RW1,CL1,CN1
      LOGICAL FLAGME,CHECK
      FLAGME=.TRUE.
      CHECK=.TRUE.

      CN1=0

      CALL DISPLAY(20,4,'Initialize Play Field')
      CALL DISPLAY(21,4,'Enter ROW=0,COL=0 to QUIT entries.')


C  Get user to enter organisms


      WHILE (FLAGME) DO

       CHECK=.TRUE.

       CALL DISPLAY (22,4,'Enter ROW,COL: ')

       READ *,RW1,CL1
       CALL DISPLAY(23,4,'                                     ')

       IF (RW1 .EQ. 0 .AND. CL1 .EQ. 0) THEN

        FLAGME=.FALSE.
        CHECK=.FALSE.


C Check for a BAD entry!

       ELSEIF (RW1 .LE. 1 .OR. RW1 .GE. 12 .OR. CL1 .LE. 1 .OR. CL1
     1.GE. 12) THEN
        CALL DISPLAY(23,4,'Incorrect Entry...Please RE-ENTER')
        CHECK=.FALSE.
        ENDIF
      IF (CHECK) THEN

            CALL DISPLAY(RW1+4,39+((CL1+2)*2),'*')

            X1(RW1,CL1)='*'

            CN1=CN1+1

       ENDIF



       CALL DISPLAY (22,4,'                           ')


      ENDWHILE

      RETURN
      END

C **********************************************************************

      SUBROUTINE RULES(BOX1,BOX2,CNT)


C **********************************************************************
C
C   Subroutine to make next generation
C
C    VARIABLES:
C
C    BOX1:  First 2-d Array
C    BOX2:  Second 2-d Array
C





      CHARACTER * 1 BOX1(12,12),BOX2(12,12)
      INTEGER R,C,E,E2,ADDEM,CNT
      CNT=0

          DO 106 R=2,11
             DO 107 C=2,11

C   Search around organism

          ADDEM=0
          IF (BOX1(R-1,C-1) .EQ. '*') ADDEM=ADDEM+1
          IF (BOX1(R,C-1) .EQ. '*') ADDEM=ADDEM+1
          IF (BOX1(R+1,C-1) .EQ. '*') ADDEM=ADDEM+1
          IF (BOX1(R-1,C) .EQ. '*') ADDEM=ADDEM+1
          IF (BOX1(R+1,C) .EQ. '*') ADDEM=ADDEM+1
          IF (BOX1(R-1,C+1) .EQ. '*') ADDEM=ADDEM+1
          IF (BOX1(R,C+1) .EQ. '*')   ADDEM=ADDEM+1
          IF (BOX1(R+1,C+1) .EQ. '*') ADDEM=ADDEM+1


               IF (BOX1(R,C) .EQ. ' ') THEN

                    IF (ADDEM .EQ. 3) THEN

                      BOX2(R,C)='*'


                      CNT=CNT+1


                    ENDIF

                ELSE

                   IF (ADDEM .EQ. 2 .OR. ADDEM .EQ. 3) THEN

                       BOX2(R,C)='*'
                       CNT=CNT+1
                   ENDIF
               ENDIF
  107 CONTINUE
  106 CONTINUE

       RETURN
       END

C   ***********************************************************************


C   ************************** ANSI SCREEN WRITES *************************
      SUBROUTINE CLS
      CHARACTER ESC
      ESC=CHAR(27)
      PRINT 10,ESC
   10 FORMAT(1x,a,'[2J')
      RETURN
      END
C   ***********************************************************************





C   ***********************************************************************
      SUBROUTINE DISPLAY(ROW,COL,MESSAGE)
      INTEGER ROW,COL
      CHARACTER*(*) MESSAGE,ESC*1
      ESC=CHAR(27)
      PRINT 10,ESC,ROW,COL,MESSAGE
   10 FORMAT (1x,a,'[',i2.2,';',i2.2,'H',(a),$)
      RETURN
      END
C   ***********************************************************************





C   ***********************************************************************
      SUBROUTINE CLEAR(ROW,COL)
      INTEGER ROW,COL
      CHARACTER ESC
      ESC=CHAR(27)
      PRINT 10,ESC,ROW,COL,ESC
   10 FORMAT (1x,a,'[',i2.2,';',i2.2,'H',a,'[K',$)
      RETURN
      END
C   ***********************************************************************



fmat$ = " \                             \   #####"
SCREEN 0, , 0, 0: CLS
SCREEN 0, , 1, 0: CLS
KEY 15, CHR$(&H0) + CHR$(73)
KEY 16, CHR$(&H20) + CHR$(73)
KEY 17, CHR$(&H0) + CHR$(81)
KEY 18, CHR$(&H20) + CHR$(81)
ON KEY(15) GOSUB 5000
ON KEY(16) GOSUB 5000
ON KEY(17) GOSUB 5500
ON KEY(18) GOSUB 5500
ON KEY(1) GOSUB 6000
ON KEY(6) GOSUB 11000
ON KEY(4) GOSUB 13000
ON KEY(5) GOSUB 11500
ON KEY(8) GOSUB 11500
ON KEY(7) GOSUB 11700
ON KEY(3) GOSUB 12000
ON KEY(2) GOSUB 12500


DIM REPLY$(100)

COLOR 15
LOCATE 22, 1
PRINT " [F1] Edit User    [F2] Find User    [F3] Jump    [F4] Delete User    [F5] Exit"





LOCATE 1, 1
COLOR 15, 0, 0
NEAT$ = "____________________________________________________"
COLOR 3
PRINT " ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ» "
PRINT " º                                                                            º "
PRINT " ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶ "
PRINT " º                                                                            º "
PRINT " º                                                                            º "
PRINT " º                                                                            º "
PRINT " º                                                                            º "
PRINT " º                                                                            º "
PRINT " º                                                                            º "
PRINT " º                                                                            º "
PRINT " º                                                                            º "
PRINT " º                                                                            º "
PRINT " º                                                                            º "
PRINT " ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶ "
PRINT " º                                                                            º "
PRINT " º                                                                            º "
PRINT " º                                                                            º "
PRINT " º                                                                            º "
PRINT " º                                                                            º "
PRINT " º                                                                            º "
PRINT " ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼ "
COLOR 10:  LOCATE 2, 5: PRINT "User Number: "

COLOR 10: LOCATE 4, 5: PRINT "Name: "; : COLOR 11: PRINT "______________________________________"
COLOR 10: LOCATE 4, 51: PRINT "Password: "; : COLOR 11: PRINT "_______________"
COLOR 10: LOCATE 6, 5: PRINT "City: "; : COLOR 11: PRINT "_______________"
COLOR 10: LOCATE 6, 27: PRINT "Data Phone: "; : COLOR 11: PRINT "___-___-____"
COLOR 10: LOCATE 6, 52: PRINT "Voice Phone: "; : COLOR 11: PRINT "___-___-____"
COLOR 10: LOCATE 8, 5: PRINT "Birth Date: "; : COLOR 11: PRINT "__-__-____"
COLOR 10: LOCATE 8, 30: PRINT "Security Level: "; : COLOR 11: PRINT "_____"
COLOR 10: LOCATE 8, 54: PRINT "Screen Length: "; : COLOR 11: PRINT "__"
COLOR 10: LOCATE 10, 5: PRINT "Ansi: "; : COLOR 11: PRINT "_"
COLOR 10: LOCATE 10, 15: PRINT "Times Called: "; : COLOR 11: PRINT "_____"
COLOR 10: LOCATE 10, 37: PRINT "Last Read: "; : COLOR 11: PRINT "_____"
COLOR 10: LOCATE 10, 57: PRINT "Screen Clearing: "; : COLOR 11: PRINT "_"
COLOR 10: LOCATE 12, 5: PRINT "Time Used Today: "; : COLOR 11: PRINT "____"
COLOR 10: LOCATE 12, 31: PRINT "Times Posted: "; : COLOR 11: PRINT "_____"
COLOR 10: LOCATE 12, 52: PRINT "Last Called: "; : COLOR 11: PRINT "__-__-____"
COLOR 12: LOCATE 15, 5: PRINT "User Upload/Download Information: "; : COLOR 11
COLOR 10: LOCATE 17, 5: PRINT "Number Of Uploads: "; : COLOR 11: PRINT "_____"
COLOR 10: LOCATE 17, 34: PRINT "Number Of Uploads In KBytes: "; : COLOR 11: PRINT "______"
COLOR 10: LOCATE 19, 5: PRINT "Number Of Downloads: "; : COLOR 11: PRINT "_____"
COLOR 10: LOCATE 19, 34: PRINT "Number Of Downloads In KBytes: "; : COLOR 11: PRINT "_____"

SCREEN 0, , 1, 1
1000 OPEN "c:\boss\users.bbs" FOR RANDOM AS #1 LEN = 190
FIELD #1, 19 AS n1$, 19 AS n2$, 15 AS p$, 19 AS C$, 12 AS v$, 12 AS b$, 1 AS AN$, 10 AS BI$, 2 AS s$, 5 AS SE$, 5 AS TC$, 5 AS LL$, 4 AS TU$, 5 AS TP$, 10 AS LC$, 5 AS U$, 7 AS UK$, 5 AS D$, 7 AS DK$, 1 AS XPERT$, 1 AS del$


GET #1, 1
usernum% = VAL(n1$)
GET #1, 2: getme% = 1

1001
LOCATE , , 0
99
GOSUB 30000
name1$ = n1$
CALL stripblanks(name1$, 3, s%)
name1$ = LEFT$(name1$, s%)

name2$ = n2$
CALL stripblanks(name2$, 3, s%)
name2$ = LEFT$(name2$, s%)

pass$ = p$
CALL stripblanks(pass$, 3, s%)
pass$ = LEFT$(pass$, s%)



city$ = C$
CALL stripblanks(city$, 3, s%)
city$ = LEFT$(city$, s%)




phone1$ = v$
phone2$ = b$
ansi$ = AN$
bdate$ = BI$
slength$ = s$


secur$ = SE$
CALL stripblanks(secur$, 3, s%)
secur$ = LEFT$(secur$, s%)



tcall$ = TC$
CALL stripblanks(tcall$, 3, s%)
tcall$ = LEFT$(tcall$, s%)






lread$ = LL$
CALL stripblanks(LL$, 3, s%)
lread$ = LEFT$(LL$, s%)




 tuse$ = TU$
CALL stripblanks(tuse$, 3, s%)
tuse$ = LEFT$(tuse$, s%)



tpost$ = TP$
CALL stripblanks(tpost$, 3, s%)
tpost$ = LEFT$(tpost$, s%)

lcall$ = LC$
 ul$ = U$
CALL stripblanks(ul$, 3, s%)
ul$ = LEFT$(ul$, s%)




ukilo$ = UK$
CALL stripblanks(ukilo$, 3, s%)
ukilo$ = LEFT$(ukilo$, s%)




 dl$ = D$
CALL stripblanks(dl$, 3, s%)
dl$ = LEFT$(dl$, s%)



 dkilo$ = DK$
CALL stripblanks(dkilo$, 3, s%)
dkilo$ = LEFT$(dkilo$, s%)


sc$ = XPERT$
del1$ = del$
COLOR 14

222 LOCATE 2, 17, 0: PRINT getme%; " of"; STR$(usernum%); "   "
GOSUB 30000
COLOR 15
LOCATE 22, 1
PRINT " [F1] Edit User    [F2] Find User    [F3] Jump    [F4] Delete User    [F5] Exit"



IF VAL(del1$) = 1 THEN COLOR 13: LOCATE 2, 60: PRINT "[DELETED]"
IF VAL(del1$) = 0 THEN LOCATE 2, 60: PRINT "         "

LOCATE 4, 11: PRINT name1$ + " " + name2$; :  PRINT LEFT$(NEAT$, 38 - LEN(name1$ + " " + name2$))

LOCATE 4, 61: PRINT pass$; :  PRINT LEFT$(NEAT$, 15 - LEN(pass$))
LOCATE 6, 11: PRINT city$; :  PRINT LEFT$(NEAT$, 20 - LEN(city$))

LOCATE 6, 39: PRINT phone1$

LOCATE 6, 65: PRINT phone2$

LOCATE 8, 17: PRINT bdate$

LOCATE 8, 46: PRINT secur$; :  PRINT LEFT$(NEAT$, 5 - LEN(secur$))

LOCATE 8, 69: PRINT slength$

LOCATE 10, 11: PRINT ansi$

LOCATE 10, 29: PRINT tcall$; :  PRINT LEFT$(NEAT$, 5 - LEN(tcall$))

LOCATE 10, 48: PRINT lread$; :  PRINT LEFT$(NEAT$, 5 - LEN(lread$))

LOCATE 10, 74: PRINT sc$

LOCATE 12, 22: PRINT tuse$; :  PRINT LEFT$(NEAT$, 4 - LEN(tuse$))

LOCATE 12, 45: PRINT tpost$; :  PRINT LEFT$(NEAT$, 5 - LEN(tpost$))

LOCATE 12, 65: PRINT lcall$

LOCATE 17, 24: PRINT ul$; :  PRINT LEFT$(NEAT$, 5 - LEN(ul$))

LOCATE 17, 63: PRINT ukilo$; :  PRINT LEFT$(NEAT$, 7 - LEN(ukilo$))

LOCATE 19, 26: PRINT dl$; :  PRINT LEFT$(NEAT$, 5 - LEN(dl$))

LOCATE 19, 65: PRINT dkilo$; :  PRINT LEFT$(NEAT$, 7 - LEN(dkilo$))

1002 KEY(15) ON: KEY(4) ON: KEY(16) ON: KEY(2) ON
KEY(17) ON: KEY(18) ON: KEY(3) ON
KEY(1) ON: KEY(5) ON: me% = 0
IF flag% = 1 THEN flag% = 0: LOCATE 1, 1, 0: GOTO 1001
IF flag% = 2 THEN flag% = 0: LOCATE 1, 1, 0: GOTO 222
IF g% = 1 THEN g% = 0: GET #1, 1 + getme%: GOTO 99
IF find% = 1 THEN find% = 0: GET #1, 1 + getme%: GOTO 99
GOTO 1002

END

5000 getme% = getme% + 1
IF getme% > usernum% THEN getme% = 1
GET #1, 1 + getme%
flag% = 1
RETURN
5500 getme% = getme% - 1
IF getme% <= 0 THEN getme% = usernum%
GET #1, 1 + getme%
flag% = 1
RETURN
6000 'edit user
GOSUB 30000

KEY(6) ON: KEY(7) ON: KEY(8) ON
LOCATE 22, 1: COLOR 15
PRINT " [F6] Save                         [F7] Abort                        [F8]  Exit"

COLOR 10
LIN% = 0
6001 LOCATE 4, 11, 1, 0, 12: MX% = 38: GOSUB 1220
IF flag% = 2 THEN GOTO 6030
IF first$ = "" AND last$ = "" THEN GOTO 1

IF first$ = "" OR last$ = "" THEN COLOR 12: LOCATE 2, 40: PRINT "First and last name must be entered.": GOTO 6001
IF LEN(first$) < 1 OR LEN(last$) <= 1 THEN COLOR 12: LOCATE 2, 40: PRINT "Name is not long enough!            ": GOTO 6001
name1$ = first$: name2$ = last$
1 GOSUB 9999
LOCATE 4, 11: PRINT name1$ + " " + name2$; MID$(NEAT$, 1, 38 - LEN(name1$ + " " + name2$))

6002 LOCATE 4, 61: MX% = 15: GOSUB 1220
IF flag% = 2 THEN GOTO 6030
IF first$ = "" AND last$ = "" THEN GOTO 2
pass$ = first$ + last$
2 LOCATE 4, 61: PRINT pass$; MID$(NEAT$, 1, 15 - LEN(pass$))

6003 LOCATE 6, 11: MX% = 19: GOSUB 1220
IF flag% = 2 THEN GOTO 6030
IF first$ = "" AND last$ = "" THEN GOTO 3
IF first$ = "" OR last$ = "" THEN COLOR 12: LOCATE 2, 40: PRINT "City and Province MUST be entered.": GOTO 6003
IF LEN(first$) < 3 OR LEN(last$) <= 1 THEN COLOR 12: LOCATE 2, 40: PRINT "City or Province not Long enough         ": GOTO 6003
city$ = first$ + "," + last$
3 GOSUB 9999
LOCATE 6, 11: PRINT city$; MID$(NEAT$, 1, 20 - LEN(city$))

6004 LOCATE 6, 39: LIN% = 1: MX% = 12: nig% = 1: GOSUB 1220: nig% = 0
IF flag% = 2 THEN GOTO 6030
IF first$ = "" AND last$ = "" THEN GOTO 4
IF LEN(first$) < 12 THEN COLOR 12: LOCATE 2, 40: PRINT "Incorrect Phone number Format.": GOTO 6004
phone1$ = first$
4 GOSUB 9999
LOCATE 6, 39: PRINT phone1$

6005 LOCATE 6, 65: LIN% = 1: MX% = 12: nig% = 1: GOSUB 1220: nig% = 0
IF flag% = 2 THEN GOTO 6030
IF first$ = "" AND last$ = "" THEN GOTO 5
IF LEN(first$) < 12 THEN COLOR 12: LOCATE 2, 40: PRINT "Incorrect Phone number Format.": GOTO 6005
phone2$ = first$
5 GOSUB 9999
LOCATE 6, 65: PRINT phone2$

6006 LOCATE 8, 17: LIN% = 1: MX% = 10: nig% = 2: GOSUB 1220: nig% = 0
IF flag% = 2 THEN GOTO 6030
IF first$ = "" AND last$ = "" THEN GOTO 6
IF LEN(first$) < 10 THEN COLOR 12: LOCATE 2, 40: PRINT "Incorrect Birth Date Format.": GOTO 6006
IF VAL(MID$(first$, 1, 2)) <= 0 OR VAL(MID$(first$, 1, 2)) > 12 THEN COLOR 12: LOCATE 2, 40: PRINT "Month Must be Between 1 and 12.": GOTO 6006
IF VAL(MID$(first$, 4, 2)) > 32 THEN COLOR 12: LOCATE 2, 40: PRINT "Day Must be Between 1 and 32.": GOTO 6006
bdate$ = first$
6 GOSUB 9999
LOCATE 8, 17: PRINT bdate$



6007 LOCATE 8, 46: LIN% = 1: MX% = 5: GOSUB 1220
IF flag% = 2 THEN GOTO 6030
IF first$ = "" AND last$ = "" THEN GOTO 7
secur$ = first$

7  LOCATE 8, 46: PRINT secur$; MID$(NEAT$, 1, 5 - LEN(secur$))
6008 LOCATE 8, 69: LIN% = 1: MX% = 2: GOSUB 1220
IF flag% = 2 THEN GOTO 6030
IF first$ = "" AND last$ = "" THEN GOTO 8
IF VAL(first$) < 10 OR VAL(first$) > 66 THEN COLOR 12: LOCATE 2, 40: PRINT "Screen Length is between 10 and 66.": GOTO 6008
slength$ = first$
8 LOCATE 8, 69: PRINT slength$


6009 LOCATE 10, 11: LIN% = 0: MX% = 1: GOSUB 1220
IF flag% = 2 THEN GOTO 6030
IF first$ = "" AND last$ = "" THEN GOTO 6010
ansi$ = MID$(first$, 1, 1): IF ansi$ = "y" OR ansi$ = "Y" THEN ansi$ = "Y": GOTO 6010
ansi$ = "N"
6010 LOCATE 10, 11: PRINT ansi$

6011 LOCATE 10, 29: LIN% = 1: MX% = 5: GOSUB 1220
IF flag% = 2 THEN GOTO 6030
IF first$ = "" AND last$ = "" THEN GOTO 11
tcall$ = first$
11 LOCATE 10, 29: PRINT tcall$; MID$(NEAT$, 1, 5 - LEN(tcall$))

6012 LOCATE 10, 48: LIN% = 1: MX% = 5: GOSUB 1220
IF flag% = 2 THEN GOTO 6030
IF first$ = "" AND last$ = "" THEN GOTO 12
lread$ = first$
12 LOCATE 10, 48: PRINT lread$; MID$(NEAT$, 1, 5 - LEN(lread$))

6013 LOCATE 10, 74: LIN% = 0: MX% = 1: GOSUB 1220
IF flag% = 2 THEN GOTO 6030
IF first$ = "" AND last$ = "" THEN GOTO 6014
sc$ = MID$(first$, 1, 1): IF sc$ = "y" OR sc$ = "Y" THEN sc$ = "Y": GOTO 6014
sc$ = "N"
6014 LOCATE 10, 74: PRINT sc$

6015 LOCATE 12, 22: LIN% = 1: MX% = 4: GOSUB 1220
IF flag% = 2 THEN GOTO 6030
IF first$ = "" AND last$ = "" THEN GOTO 14
tuse$ = first$
14 LOCATE 12, 22: PRINT tuse$; MID$(NEAT$, 1, 4 - LEN(tuse$))

6016 LOCATE 12, 45: LIN% = 1: MX% = 5: GOSUB 1220
IF flag% = 2 THEN GOTO 6030
IF first$ = "" AND last$ = "" THEN GOTO 15
tpost$ = first$
15 LOCATE 12, 45: PRINT tpost$; MID$(NEAT$, 1, 5 - LEN(tpost$))

6019 LOCATE 12, 65: LIN% = 1: MX% = 10: nig% = 2: GOSUB 1220: nig% = 0
IF flag% = 2 THEN GOTO 6030
IF first$ = "" AND last$ = "" THEN GOTO 16
IF LEN(first$) < 10 THEN COLOR 12: LOCATE 2, 40: PRINT "Incorrect Birth Date Format.": GOTO 6019
IF VAL(MID$(first$, 1, 2)) <= 0 OR VAL(MID$(first$, 1, 2)) > 12 THEN COLOR 12: LOCATE 2, 40: PRINT "Month Must be Between 1 and 12.": GOTO 6019
IF VAL(MID$(first$, 4, 2)) > 32 THEN COLOR 12: LOCATE 2, 40: PRINT "Day Must be Between 1 and 32.": GOTO 6019
lcall$ = first$
16 GOSUB 9999
LOCATE 12, 65: PRINT lcall$


6025 LOCATE 17, 24: LIN% = 1: MX% = 5: GOSUB 1220
IF flag% = 2 THEN GOTO 6030
IF first$ = "" AND last$ = "" THEN GOTO 17
ul$ = first$
17 LOCATE 17, 24: PRINT ul$; MID$(NEAT$, 1, 5 - LEN(ul$))

6026 LOCATE 17, 63: LIN% = 1: MX% = 7: GOSUB 1220
IF flag% = 2 THEN GOTO 6030
IF first$ = "" AND last$ = "" THEN GOTO 18
ukilo$ = first$
18 LOCATE 17, 63: PRINT ukilo$; MID$(NEAT$, 1, 7 - LEN(ukilo$))



6027 LOCATE 19, 26: LIN% = 1: MX% = 5: GOSUB 1220
IF flag% = 2 THEN GOTO 6030
IF first$ = "" AND last$ = "" THEN GOTO 22
dl$ = first$
22 LOCATE 19, 26: PRINT dl$; MID$(NEAT$, 1, 5 - LEN(dl$))


6028 LOCATE 19, 65: LIN% = 1: MX% = 7: GOSUB 1220
IF flag% = 2 THEN GOTO 6030
IF first$ = "" AND last$ = "" THEN GOTO 23
dkilo$ = first$
23 LOCATE 19, 65: PRINT dkilo$; MID$(NEAT$, 1, 7 - LEN(dkilo$))



6030

 IF abort% = 1 THEN RETURN
 LSET n1$ = name1$
 LSET n2$ = name2$
 LSET p$ = pass$
 LSET C$ = city$
 LSET v$ = phone1$
 LSET b$ = phone2$
 LSET AN$ = ansi$
 LSET BI$ = bdate$
 LSET s$ = slength$
 LSET SE$ = secur$
 LSET TC$ = tcall$
 LSET LL$ = lread$
 LSET TU$ = tuse$
 LSET TP$ = tpost$
 LSET LC$ = lcall$
 LSET U$ = ul$
 LSET UK$ = ukilo$
 LSET D$ = dl$
 LSET DK$ = dkilo$
 LSET XPERT$ = sc$
 LSET del$ = del1$


flag% = 2
PUT #1, 1 + getme%

 CLOSE #1
OPEN "c:\boss\users.bbs" FOR RANDOM AS #1 LEN = 190
FIELD #1, 19 AS n1$, 19 AS n2$, 15 AS p$, 19 AS C$, 12 AS v$, 12 AS b$, 1 AS AN$, 10 AS BI$, 2 AS s$, 5 AS SE$, 5 AS TC$, 5 AS LL$, 4 AS TU$, 5 AS TP$, 10 AS LC$, 5 AS U$, 7 AS UK$, 5 AS D$, 7 AS DK$, 1 AS XPERT$, 1 AS del$

KEY(6) OFF

RETURN

END


9999 LOCATE 2, 40: PRINT "                                       ": RETURN

10260 t% = 0: ca$ = ""
C% = LEN(CT$)
10280 IF C% - t% < 1 THEN RETURN
IF MID$(CT$, C% - t%, 1) = CHR$(32) THEN t% = t% + 1: GOTO 10280
FOR I = 1 TO C% - t%: ca$ = ca$ + MID$(CT$, I, 1): NEXT I: CT$ = ca$: RETURN



1220 l% = 0: BIG% = 0: a$ = "": first$ = "": last$ = ""
1230 COLOR 11
1240 a$ = INKEY$
IF flag% = 2 THEN RETURN
IF a$ = CHR$(8) AND l% > 0 THEN l% = l% - 1: IF l% >= 0 THEN GOSUB 1420: COLOR 11: GOTO 1240
1270 IF l% <= 0 THEN l% = 0
IF a$ = CHR$(13) THEN GOSUB 1470: RETURN
1290 IF a$ = "," AND l% < MX% THEN TACK% = 1: t$ = ",": l% = l% + 1: REPLY$(l%) = " ": GOSUB 1580: GOTO 1240
1300 IF LIN% = 1 AND a$ < "0" OR LIN% = 1 AND a$ > "9" THEN GOTO 1240
IF a$ = "" OR a$ < CHR$(32) OR a$ > CHR$(123) OR l% >= MX% OR l% < 0 THEN GOTO 1240
1320 l% = l% + 1
IF nig% = 1 AND l% = 4 OR nig% = 1 AND l% = 8 THEN REPLY$(l%) = "-": PRINT "-"; : l% = l% + 1
IF nig% = 2 AND l% = 3 OR nig% = 2 AND l% = 6 THEN REPLY$(l%) = "-": PRINT "-"; : l% = l% + 1
1330 REPLY$(l%) = a$
IF l% > 1 AND REPLY$(l% - 1) = CHR$(32) THEN BIG% = 1: GOTO 1360
BIG% = 0
1360 IF l% = 1 OR BIG% = 1 THEN GOSUB 1430: GOTO 1380
1370 GOSUB 1450
1380 REPLY$(l%) = a$
IF PAS% = 0 THEN t$ = a$: TACK% = 1: GOSUB 1580
IF PAS% = 1 THEN t$ = "X": TACK% = 1: GOSUB 1580
GOTO 1240
1420 COLOR 11: t$ = CHR$(29): GOSUB 1571: t$ = "_": GOSUB 1571: t$ = CHR$(29): GOSUB 1571: RETURN
1430 R% = ASC(a$): IF R% >= 97 AND R% <= 122 THEN R% = R% - 32: a$ = CHR$(R%): RETURN
RETURN
1450 R% = ASC(a$): IF R% >= 65 AND R% <= 90 THEN R% = R% + 32: a$ = CHR$(R%): RETURN
RETURN
1470 SPAC% = 0: SPAC2% = 0: F$ = "": last$ = "": first$ = "": FOR I% = 1 TO l%
IF SPAC% = 0 AND REPLY$(I%) = CHR$(32) THEN GOTO 1540
IF SPAC% = 1 AND REPLY$(I%) = CHR$(32) THEN SPAC2% = 1
IF SPAC2% = 1 THEN F$ = REPLY$(I%): IF LEN(last$) > 0 AND F$ = CHR$(32) THEN I% = l%: GOTO 1550
IF SPAC2% = 1 AND REPLY$(I%) = CHR$(32) THEN GOTO 1540
IF SPAC2% = 1 THEN F$ = REPLY$(I%): last$ = last$ + F$
IF SPAC2% = 0 THEN F$ = REPLY$(I%): first$ = first$ + F$: SPAC% = 1

1540 REM
1550 NEXT I%
RETURN
1571 PRINT t$;
RETURN
1580 PRINT t$;
RETURN
11000 flag% = 2: RETURN

11500 RESET: SCREEN 0, , 0, 0: CLS : SYSTEM
11700 flag% = 2: abort% = 1: RETURN
12000 SCREEN 0, , 0, 0: CLS
GOSUB 30000
12001 LOCATE 1, 1
   IF me% = 1 THEN GOSUB 20001: GOTO 12002
COLOR 12: PRINT "Enter User Number [1 -" + STR$(usernum%) + "]: "; : COLOR 11: PRINT "_____": LOCATE 1, 30, 1, 0, 12
12002  LIN% = 1: MX% = 5: GOSUB 1220
      IF first$ = "" THEN LOCATE , , 0: GOTO 12004
     IF VAL(first$) > usernum% AND me% = 1 THEN LOCATE a, 40: PRINT "Choice is between 1 and"; usernum%: GOTO 12001
     IF VAL(first$) > usernum% THEN LOCATE 3, 1: PRINT "Choice is between 1 and"; usernum%: GOTO 12001
      g% = 1: getme% = VAL(first$)

12004 SCREEN 0, , 1, 1
RETURN

12500 SCREEN 0, , 0, 0: CLS : me% = 1: GOSUB 30000
COLOR 10: LOCATE 1, 1: PRINT "Enter Partial or Full Name: ";
COLOR 11: PRINT "________________________________________": LOCATE 1, 29, 1, 0, 12
COLOR 11: LIN% = 0: MX% = 38: GOSUB 1220
 IF first$ = "" THEN LOCATE , , 0: SCREEN 0, , 1, 1: RETURN
PRINT : PRINT : PRINT "  User Name                      User Number": PRINT "  =========                      ===========": PRINT
  COLOR 14: l% = 0: CALL upcase(first$): CALL upcase(last$)
87  l% = l% + 1: IF l% > usernum% THEN PRINT : GOSUB 20000: GOTO 12002
  GET #1, 1 + l%

  name1$ = n1$: name2$ = n2$
CALL stripblanks(name1$, 3, s%)
name1$ = LEFT$(name1$, s%)
CALL stripblanks(name2$, 3, s%)
name2$ = LEFT$(name2$, s%)


  CALL upcase(n1$)
  CALL upcase(n2$)

 ' print n1$,n2$,first$,last$


IF first$ <> "" AND last$ <> "" AND INSTR(n1$, first$) AND INSTR(n2$, last$) THEN PRINT USING fmat$; name1$ + " " + name2$; l%: GOTO 87
IF INSTR(n1$, first$) AND last$ = "" OR INSTR(n2$, first$) AND last$ = "" THEN PRINT USING fmat$; name1$ + " " + name2$; l%: GOTO 87
GOTO 87


20000 COLOR 12: PRINT "Enter User Number [1 -" + STR$(usernum%) + "]: ";
     a = CSRLIN: b = POS(0)

20001  LOCATE a, b: COLOR 11: PRINT "_____"
     IF a = 24 THEN a = 23
     LOCATE a, b, 1, 0, 12
     RETURN

30000 KEY(1) STOP: KEY(2) STOP: KEY(3) STOP: KEY(5) STOP
     KEY(6) STOP: KEY(7) STOP: KEY(8) STOP: KEY(9) STOP
     KEY(10) STOP
     KEY(15) STOP: KEY(16) STOP: KEY(17) STOP: KEY(18) STOP
     RETURN

13000


     IF VAL(del1$) = 0 THEN del1$ = "1": GOTO 13001
      IF VAL(del1$) = 1 THEN del1$ = ""
13001 abort% = 0: GOSUB 6030

RETURN


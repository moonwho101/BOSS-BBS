tt$ = COMMAND$
tt1$ = MID$(tt$, 1, 2)
tt2$ = MID$(tt$, 4, 2)
tt3$ = MID$(tt$, 7, 2)


SCREEN 0, 0, 0: CLS
del% = 0

DIM num%(6000), sup%(6000)
DIM p$(900), d$(900)

OPEN "index.bbs" FOR RANDOM AS #1 LEN = 30
FIELD #1, 5 AS message$, 5 AS head$, 10 AS sloc$, 10 AS lloc$

OPEN "message.bbs" FOR RANDOM AS #2 LEN = 72
FIELD #2, 72 AS savemes$

OPEN "header.bbs" FOR RANDOM AS #3 LEN = 125
FIELD #3, 5 AS msg$, 2 AS ar$, 1 AS sta$, 1 AS reci$, 8 AS dat$, 11 AS tyme$, 19 AS to1$, 19 AS to2$, 19 AS from$, 35 AS sj$, 5 AS tra$

OPEN "index.tem" FOR RANDOM AS #4 LEN = 30
FIELD #4, 5 AS message1$, 5 AS head1$, 10 AS sloc1$, 10 AS lloc1$

OPEN "message.tem" FOR RANDOM AS #5 LEN = 72
FIELD #5, 72 AS savemes1$

OPEN "header.tem" FOR RANDOM AS #6 LEN = 125
FIELD #6, 5 AS msg1$, 2 AS ar1$, 1 AS sta1$, 1 AS reci1$, 8 AS dat1$, 11 AS tyme1$, 19 AS to11$, 19 AS to21$, 19 AS from1$, 35 AS sj1$, 5 AS tra1$


DEFSNG A-F
c% = 0: d% = 1: e% = 0: f% = 0
COLOR 14
LOCATE 4, 1: PRINT "Packing Message Base"
COLOR 11
GET #1, 1
mesnum$ = message$

FOR a = 1 TO VAL(mesnum$)


GET #1, a + 1


GET #3, VAL(head$)


tt4$ = MID$(dat$, 1, 2)
tt5$ = MID$(dat$, 4, 2)
tt6$ = MID$(dat$, 7, 2)

LOCATE 6, 1

IF MID$(ar$, 1, 1) = "D" THEN find% = 1: del% = del% + 1: PRINT "Msg # " + msg$ + "deleted": GOTO 40

IF MID$(sta$, 1, 1) = "Y" THEN find% = 1: del% = del% + 1: PRINT "Msg # " + msg$ + "deleted": GOTO 40



REM IF VAL(tt4$) < VAL(tt1$) AND VAL(tt6$) <= VAL(tt3$) THEN find% = 1: del% = del% + 1: PRINT "Msg # " + msg$ + "deleted": GOTO 40
REM IF VAL(tt4$) = VAL(tt1$) AND VAL(tt5$) <= VAL(tt2$) AND VAL(tt6$) <= VAL(tt3$) THEN find% = 1: del% = del% + 1: PRINT "Msg # " + msg$ + "deleted": GOTO 40




IF INSTR(tra$, "Root") THEN a1$ = "Root": GOTO 91
r = VAL(tra$)
IF sup%(r) = 0 THEN a1$ = "Null": GOTO 91
a1$ = MID$(STR$(sup%(r)), 2, LEN(STR$(sup%(r))))

91



 c% = c% + 1


LOCATE 6, 1

PRINT a; " -->"; c%; "       ";

LSET message1$ = MID$(STR$(c%), 2, LEN(STR$(c%)))
LSET head1$ = MID$(STR$(c%), 2, LEN(STR$(c%)))
LSET sloc1$ = MID$(STR$(d%), 2, LEN(STR$(d%)))
e% = VAL(lloc$) - VAL(sloc$)
e% = e% + d%
LSET lloc1$ = MID$(STR$(e%), 2, LEN(STR$(e%)))


PUT #4, c% + 1
l% = 0
     FOR b = VAL(sloc$) TO VAL(lloc$)
	GET #2, b
	LSET savemes1$ = savemes$

l% = l% + 1


PUT #5, d% + l% - 1
     NEXT b

d% = e%
d% = d% + 1

LSET msg1$ = MID$(STR$(c%), 2, LEN(STR$(c%)))
LSET ar1$ = ar$
LSET sta1$ = sta$
LSET reci1$ = reci$
LSET dat1$ = dat$
LSET tyme1$ = tyme$
LSET to11$ = to1$
LSET to21$ = to2$
LSET from1$ = from$
LSET sj1$ = sj$

LSET tra1$ = a1$



PUT #6, c%


40
IF find% = 1 THEN num%(a) = a: sup%(a) = 0: find% = 0: GOTO 50
num%(a) = a: sup%(a) = c%

50


NEXT a


LSET message1$ = STR$(c%)
LSET head1$ = ""
LSET sloc1$ = ""
LSET lloc1$ = ""



PUT #4, 1

CLOSE #1, 2, 3, 4, 5, 6
RESET


OPEN DRI$ + "USERS.BBS" FOR RANDOM AS #1 LEN = 190: FIELD #1, 10 AS number$: GET #1, 1: number% = VAL(number$): CLOSE #1


OPEN "LASTREAD.BBS" FOR RANDOM AS #1 LEN = 100
FIELD #1, 5 AS p$(1), 5 AS p$(2), 5 AS p$(3), 5 AS p$(4), 5 AS p$(5), 5 AS p$(6), 5 AS p$(7), 5 AS p$(8), 5 AS p$(9), 5 AS p$(10), 5 AS p$(11), 5 AS p$(12), 5 AS p$(13), 5 AS p$(14), 5 AS p$(15), 5 AS p$(16), 5 AS p$(17), 5 AS p$(18), 5 AS p$(19), 5 _
 AS p$(20)

OPEN "LASTREAD.TEM" FOR RANDOM AS #2 LEN = 100
FIELD #2, 5 AS d$(1), 5 AS d$(2), 5 AS d$(3), 5 AS d$(4), 5 AS d$(5), 5 AS d$(6), 5 AS d$(7), 5 AS d$(8), 5 AS d$(9), 5 AS d$(10), 5 AS d$(11), 5 AS d$(12), 5 AS d$(13), 5 AS d$(14), 5 AS d$(15), 5 AS d$(16), 5 AS d$(17), 5 AS d$(18), 5 AS d$(19), 5 _
 AS d$(20)
COLOR 14
LOCATE 8, 1

PRINT "Updating Lastread Pointers"
COLOR 11

FOR i = 1 TO number%


GET #1, i


    FOR z = 1 TO 20
	LOCATE 10, 1
	PRINT p$(z); " --> ";

IF sup%(VAL(p$(z))) > 0 THEN LSET d$(z) = MID$(STR$(sup%(VAL(p$(z)))), 2, LEN(STR$(sup%(VAL(p$(z)))))): PRINT MID$(STR$(sup%(VAL(p$(z)))), 2, LEN(STR$(sup%(VAL(p$(z)))))); " ";
IF sup%(VAL(p$(z))) = 0 THEN GOSUB 9000: LSET d$(z) = MID$(STR$(fin), 2, LEN(STR$(fin))): PRINT fin; " ";



  NEXT z

PUT #2, i

NEXT i
CLOSE #1, 2, 3, 4, 5

OPEN "USERS.BBS" FOR RANDOM AS #1 LEN = 190
FIELD #1, 19 AS n1$, 19 AS n2$, 15 AS p$, 19 AS c$, 12 AS v$, 12 AS b$, 1 AS an$, 10 AS bi$, 2 AS s$, 5 AS se$, 5 AS tc$, 5 AS ll$, 4 AS tu$, 5 AS tp$, 10 AS lc$, 5 AS u$, 7 AS uk$, 5 AS d$, 7 AS dk$, 1 AS xpert$, 1 AS del$

OPEN "USERS.TEM" FOR RANDOM AS #2 LEN = 190
FIELD #2, 19 AS n11$, 19 AS n21$, 15 AS p1$, 19 AS c1$, 12 AS v1$, 12 AS b1$, 1 AS an1$, 10 AS bi1$, 2 AS s1$, 5 AS se1$, 5 AS tc1$, 5 AS ll1$, 4 AS tu1$, 5 AS tp1$, 10 AS lc1$, 5 AS u1$, 7 AS uk1$, 5 AS d1$, 7 AS dk1$, 1 AS xpert1$, 1 AS DEL1$


COLOR 14
LOCATE 12, 1
PRINT "Updating USERS.BBS scan Number"
COLOR 11


FOR z = 1 TO number%

GET #1, z + 1

LOCATE 14, 1
PRINT ll$; " --> ";

LSET n11$ = n1$
LSET n21$ = n2$
LSET p1$ = p$
LSET c1$ = c$
LSET v1$ = v$
LSET b1$ = b$
LSET an1$ = an$
LSET bi1$ = bi$
LSET s1$ = s$
LSET se1$ = se$
LSET tc1$ = tc$


IF sup%(VAL(ll$)) > 0 THEN LSET ll1$ = MID$(STR$(sup%(VAL(ll$))), 2, LEN(STR$(sup%(VAL(ll$))))): PRINT ll1$; " ";
IF sup%(VAL(ll$)) = 0 THEN GOSUB 10000: LSET ll1$ = MID$(STR$(fin), 2, LEN(STR$(fin))): PRINT fin; " ";



LSET tu1$ = tu$
LSET tp1$ = tp$
LSET lc1$ = lc$
LSET u1$ = u$
LSET uk1$ = uk$
LSET d1$ = d$
LSET dk1$ = dk$
LSET xpert1$ = xpert$
LSET DEL1$ = del$

PUT #2, z + 1

NEXT z



LSET n11$ = STR$(number%)
LSET n21$ = ""
LSET p1$ = ""
LSET c1$ = ""
LSET v1$ = ""
LSET b1$ = ""
LSET an1$ = ""
LSET bi1$ = ""
LSET s1$ = ""
LSET se1$ = ""
LSET tc1$ = ""
LSET ll1$ = ""
LSET tu1$ = ""
LSET tp1$ = ""
LSET lc1$ = ""
LSET u1$ = ""
LSET uk1$ = ""
LSET d1$ = ""
LSET dk1$ = ""
LSET xpert1$ = ""
LSET DEL1$ = ""

PUT #2, 1

CLOSE #1, 2, 3, 4, 5, 6
RESET


KILL "*.bbs"

NAME "users.tem" AS "users.bbs"
NAME "lastread.tem" AS "lastread.bbs"
NAME "message.tem" AS "message.bbs"
NAME "index.tem" AS "index.bbs"
NAME "header.tem" AS "header.bbs"

COLOR 14
LOCATE 16, 1

PRINT del%; " message(s) deleted."





SYSTEM




9000 x = VAL(p$(z))
9001 x = x - 1
     IF x <= 0 THEN fin = 0: RETURN
  IF sup%(x) = 0 THEN GOTO 9001

  fin = sup%(x)

RETURN


10000 x = (VAL(ll$))
10001 x = x - 1
     IF x <= 0 THEN fin = 0: RETURN
 IF sup%(x) = 0 THEN GOTO 10001
 fin = sup%(x)

RETURN


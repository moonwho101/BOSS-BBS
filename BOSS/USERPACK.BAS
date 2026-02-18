SCREEN 0, 0, 0: CLS
del% = 0
dim d$(20),p$(20)

COLOR 14
LOCATE 1, 1: PRINT "Packing User File"
COLOR 11


OPEN DRI$ + "USERS.BBS" FOR RANDOM AS #1 LEN = 190: FIELD #1, 10 AS number$: GET #1, 1: number% = VAL(number$): CLOSE #1


OPEN "LASTREAD.BBS" FOR RANDOM AS #1 LEN = 100
FIELD #1, 5 AS p$(1), 5 AS p$(2), 5 AS p$(3), 5 AS p$(4), 5 AS p$(5), 5 AS p$(6), 5 AS p$(7), 5 AS p$(8), 5 AS p$(9), 5 AS p$(10), 5 AS p$(11), 5 AS p$(12), 5 AS p$(13), 5 AS p$(14), 5 AS p$(15), 5 AS p$(16), 5 AS p$(17), 5 AS p$(18), 5 AS p$(19), 5 _
 AS p$(20)

OPEN "LASTREAD.TEM" FOR RANDOM AS #2 LEN = 100

FIELD #2, 5 AS d$(1), 5 AS d$(2), 5 AS d$(3), 5 AS d$(4), 5 AS d$(5), 5 AS d$(6), 5 AS d$(7), 5 AS d$(8), 5 AS d$(9), 5 AS d$(10), 5 AS d$(11), 5 AS d$(12), 5 AS d$(13), 5 AS d$(14), 5 AS d$(15), 5 AS d$(16), 5 AS d$(17), 5 AS d$(18), 5 AS d$(19), 5 _
 AS d$(20)


OPEN "USERS.BBS" FOR RANDOM AS #3 LEN = 190
FIELD #3, 19 AS n1$, 19 AS n2$, 15 AS p$, 19 AS c$, 12 AS v$, 12 AS b$, 1 AS an$, 10 AS bi$, 2 AS s$, 5 AS se$, 5 AS tc$, 5 AS ll$, 4 AS tu$, 5 AS tp$, 10 AS lc$, 5 AS u$, 7 AS uk$, 5 AS d$, 7 AS dk$, 1 AS xpert$,1 as del$

OPEN "USERS.TEM" FOR RANDOM AS #4 LEN = 190
FIELD #4, 19 AS n11$, 19 AS n21$, 15 AS p1$, 19 AS c1$, 12 AS v1$, 12 AS b1$, 1 AS an1$, 10 AS bi1$, 2 AS s1$, 5 AS se1$, 5 AS tc1$, 5 AS ll1$, 4 AS tu1$, 5 AS tp1$, 10 AS lc1$, 5 AS u1$, 7 AS uk1$, 5 AS d1$, 7 AS dk1$, 1 AS xpert1$,1 as del1$

o=0
ar%=0

FOR z = 1 TO number%

 GET #3, z + 1



if val(del$)=1 then gosub 12000:goto 1000





o=o+1




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
LSET LL1$=LL$
LSET tu1$ = tu$
LSET tp1$ = tp$
LSET lc1$ = lc$
LSET u1$ = u$
LSET uk1$ = uk$
LSET d1$ = d$
LSET dk1$ = dk$
LSET xpert1$ = xpert$
LSET del1$=del$


PUT #4, o + 1

GET #1, z





FOR i = 1 TO 20

       lset d$(i)=p$(i)


    NEXT i

PUT #2, o




1000 NEXT z



LSET n11$ = STR$(number%-del%)
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
LSET DEL1$=""

PUT #4, 1

CLOSE #1, 2, 3, 4, 5, 6
RESET


KILL "users.bbs"
KILL "lastread.bbs"

NAME "users.tem" AS "users.bbs"
NAME "lastread.tem" AS "lastread.bbs"


COLOR 14
print:print

PRINT del%; " users(s) deleted."

COLOR 15

print
COLOR 10
PRINT "[Press Return]:";


11 a$ = INKEY$
IF a$ = "" THEN GOTO 11

SYSTEM


12000 if ar%=0 then ar%=1:color 11:print:print "Deleting:":color 10:print

nama1$=n1$
nama2$=n2$

call stripblanks(nama1$,3,S%)
nama1$=left$(nama1$,s%)
call stripblanks(nama2$,3,S%)
nama2$=left$(nama2$,s%)

print nama1$+" "+nama2$+"         "
del%=del%+1

return


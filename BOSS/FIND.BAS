r$="### \                    \SE:\ \ DL: \            \ UL: \            \ M:\     \"

SCREEN 0, 0, 0: CLS
del% = 0
dim d$(20),p$(20)

COLOR 14

COLOR 11


OPEN DRI$ + "USERS.BBS" FOR RANDOM AS #1 LEN = 190: FIELD #1, 10 AS number$: GET #1, 1: number% = VAL(number$): CLOSE #1



OPEN "USERS.BBS" FOR RANDOM AS #3 LEN = 190
FIELD #3, 19 AS n1$, 19 AS n2$, 15 AS p$, 19 AS c$, 12 AS v$, 12 AS b$, 1 AS an$, 10 AS bi$, 2 AS s$, 5 AS se$, 5 AS tc$, 5 AS ll$, 4 AS tu$, 5 AS tp$, 10 AS lc$, 5 AS u$, 7 AS uk$, 5 AS d$, 7 AS dk$, 1 AS xpert$,1 as del$




FOR z = 1 TO number%

 GET #3, z + 1


if instr(se$,"9") then gosub 9000



1000 NEXT z
end


9000
call stripblanks(n1$,3,S%)
a$=left$(n1$,s%)

call stripblanks(n2$,3,S%)
b$=left$(n2$,s%)




print using r$;z,a$+" "+b$,se$,dk$,uk$,tp$

return




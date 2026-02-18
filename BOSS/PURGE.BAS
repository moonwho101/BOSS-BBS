open "C:\temp\purge.rpt" for input as #1

OPEN "USERS.BBS" FOR RANDOM AS #3 LEN = 190
FIELD #3, 19 AS n1$, 19 AS n2$, 15 AS p$, 19 AS c$, 12 AS v$, 12 AS b$, 1 AS an$, 10 AS bi$, 2 AS s$, 5 AS se$, 5 AS tc$, 5 AS ll$, 4 AS tu$, 5 AS tp$, 10 AS lc$, 5 AS u$, 7 AS uk$, 5 AS d$, 7 AS dk$, 1 AS xpert$,1 as del$



while not eof(1)

line input #1,a$

b$=mid$(a$,1,25)

x%= val(mid$(a$,55,20))

if x%=0 then goto 10

get #3,x%+1

LSET del$="1"

print b$,x%,n1$

PUT #3, x% + 1

10

wend

reset:close #1,2,3,4


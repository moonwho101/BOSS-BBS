
q$="c:\boss\file"

q$=q$+command$



DIM FL$(500)
DIM GT$(500)
FIL$=q$+"\*.*"
CALL FINDFIRSTF ( FIL$+CHR$(0),0,EC%)

L%=0
10


FL$=SPACE$(12)
CALL GETNAMEF(FL$,FLEN%)
FIL$=LEFT$(FL$,FLEN%)
L%=L%+1



FL$(L%)= FIL$

CALL FINDNEXTF(EC%)


IF EC%<>0 THEN GOTO 20

GOTO 10


20  OPEN q$+"\FILES.BBS" FOR INPUT AS #1

  R%=0

   WHILE NOT EOF(1)

    R%=R%+1
  INPUT #1,A$:line INPUT #1,B$
    call stripblanks(A$,3,slen%)
    a$=left$(a$,slen%)

   call upcase(a$)

  GT$(R%)=A$

  WEND
       CLOSE #1

   open q$+"\files.bbs" for append as #1



   for i=1 to l%

      for z=1 to r%


   if gt$(z)=fl$(i) then flg%=1


    next z

  if flg%=1 then flg%=0 goto 2000
   gosub 9000

2000

next i
close #1

end
9000 print fl$(i);" was not found in list, add it [y/n]: ";

  9001 a$=inkey$:if a$="" goto 9001

  if a$="y" or a$="Y" then goto 9010
   print "N"
 return

9010
   print a$
print "                   [-----------------------------------------]

line input  "Enter Description:  ",a$


print #1,fl$(i);",";a$

return



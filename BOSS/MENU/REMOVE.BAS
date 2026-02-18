on error goto 9000


for i=0 to 99

x$=str$(i)





f$="menu"+mid$(x$,2,len(x$))+"-4"+".asc"

open f$ for input as #1

open "dd" for output as #2

while not eof(1)

line input #1,r$

x=instr(r$,chr$(1))
if x>0 then mid$(r$,x,3)="   "

print #2,r$


wend

close #1
close #2

kill f$
name "dd" as f$
5


next i


9000 print err
if err=53 then resume 5

11

{
  ***********************************************************************
  *     DEQUE.PAS,  27-242, Ass. #2 , 21-FEB-90, Mark Longo 880998420   *
  *=====================================================================*
  *                                                                     *
  *  This is a double ended queue unit is Turbo Pascal.  The user       *
  *  simply puts DEQUE under the USES part of his/her program and       *
  *  they will have access to the following functions:                  *
  *                                                                     *
  *  deque_create        :  Creates a deque.                            *
  *  deque_push_right    :  Add an element to the right of the deque    *
  *  deque_push_left     :  Add an element to the left of the deque     *
  *  deque_pop_right     :  Pull an element from the right of the deque *
  *  deque_pop_left      :  Pull an element from the left of the deque  *
  *  deque_print_char    :  Display a deque of CHAR to the screen       *
  *                                                                     *
  ***********************************************************************


}

10
Unit deque;

INTERFACE

const

DEQUE_MAX_LENGTH = 5;

type

   element_type = char;
   nodeptr      = ^node;
   node         = record
                  name      : element_type;
                  prev,next : nodeptr;
                  end;

   deque_type   = record
                  left,right      : nodeptr;
                  number_elements : word;
                  end;
12
procedure deque_create (var x: deque_type);
procedure deque_push_right (element: element_type; var x: deque_type);
procedure deque_push_left(element: element_type; var x : deque_type);
procedure deque_pop_right (var element : element_type; var x: deque_type);
procedure deque_pop_left(var element: element_type; var x: deque_type);
procedure deque_print_char (var x:deque_type);

11
IMPLEMENTATION

{***********************************************************************}

procedure deque_check(var x:deque_type);

{Checks to see if maximum deque length was exceded!}

   begin
   if (x.number_elements) >= (DEQUE_MAX_LENGTH) then
       begin
       writeln;
       writeln('Deque Error:  Exceded deque maximum length');
       halt;
       end;
   end;

{***********************************************************************}

procedure deque_create (var x: deque_type);

{Procedure to create a deque}

   begin
   x.left  := nil;
   x.right := nil;
   x.number_elements :=0;
   end;

{***********************************************************************}

procedure deque_push_right (element: element_type; var x: deque_type);

{Push an element to the right of the deque}

   var
      temp,prev : nodeptr;

   begin
   deque_check(x);
   if x.number_elements = 0 then
      begin
      new(temp);
      temp^.name := element;
      temp^.prev := nil;
      temp^.next := nil;
      inc(x.number_elements);
      x.left :=temp;
      x.right := temp;
      end
   else
      begin
      prev := x.right;
      new(temp);
      prev^.next := temp;
      temp^.prev := prev;
      temp^.next := nil;
      temp^.name := element;
      inc (x.number_elements);
      x.right := temp;
      end;
   end;

{***********************************************************************}

procedure deque_push_left(element: element_type; var x : deque_type);

{Push an element to the left of a deque}

   var
      temp,prev : nodeptr;

   begin
   deque_check(x);
   if x.number_elements = 0 then
      begin
      new(temp);
      temp^.name := element;
      temp^.prev := nil;
      temp^.next := nil;
      inc (x.number_elements);
      x.left := temp;
      x.right := temp;
      end
   else
      begin
      prev := x.left;
      new (temp);
      prev^.prev := temp;
      temp^.next := prev;
      temp^.prev := nil;
      temp^.name := element;
      inc (x.number_elements);
      x.left := temp;
      end;
   end;

{***********************************************************************}

procedure deque_pop_right (var element : element_type; var x: deque_type);

{Pop an element from the right of a deque}

   var

      temp,prev: nodeptr;

   begin
   if x.number_elements = 0 then
      begin
      writeln;
      writeln('Deque Error: You tried to POP an empty deque! [deque_pop_right]');
      halt;
      end
   else
      begin
      prev := x.right;
      if (prev^.prev = nil) then
         begin
         element := prev^.name;
         dispose (prev);
         x.right := nil;
         x.left := nil;
         x.number_elements := 0;
         end
      else
         begin
         temp := prev^.prev;
         dec(x.number_elements);
         element := prev^.name;
         temp^.next := nil;
         dispose (prev);
         x.right := temp;
         end;
      end;
   end;

{***********************************************************************}

procedure deque_pop_left(var element: element_type; var x: deque_type);

{Pop element from left of deque}

   var
      temp,prev: nodeptr;

   begin
   if x.number_elements = 0 then
      begin
      writeln;
      writeln('Deque Error : You tried to POP an empty deque! [deque_pop_left]');
      halt;
      end
   else
      begin
      prev := x.left;
      if (prev^.next = nil) then
         begin
         element := prev^.name;
         dispose (prev);
         x.right := nil;
         x.left := nil;
         x.number_elements := 0;
         end
      else
         begin
         temp := prev^.next;
         dec(x.number_elements);
         element := prev^.name;
         temp^.prev := nil;
         dispose (prev);
         x.left := temp;
         end;
      end;
   end;

{***********************************************************************}

procedure deque_print_char (var x:deque_type);

{Print out the deque}

   var
      temp    : nodeptr;
      counter : word;

   begin
   if x.number_elements = 0 then
      begin
      writeln;
      writeln('  Deque is empty.');
      end
   else
      begin
      temp := x.left;
      writeln;
      write('      ');
      for counter :=1 to x.number_elements do
         begin
         write(char(218),char(196),char(196),char(196));
         write(char(191),'  ');
         end;
      writeln;
      write ('  Nil',char(27));
      for counter :=1 to x.number_elements-1 do
         begin
         write (char(179));
         write(' ',temp^.name,' ');
         temp := temp^.next;
         write(char(179),char(27),char(26));
         end;
      write(char(179));
      write(' ',temp^.name,' ');
      write(char(179),char(26),'Nil');
      writeln;
      write('      ');
      for counter :=1 to x.number_elements do
         begin
         write(char(192),char(196),char(196),char(196));
         write(char(217),'  ');
         end;
   end;
   end;
END.

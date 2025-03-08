{Helper program to translate SHG::Lin_Dioph::intrank() and
SHG::Lin_Dioph::solveinteger() from Algol 68 to C++. See
include/shg/mathutils.h.}

program solveint;

const
  {constants for solveinteger}
  maxm1 = 9; {maximum number of equations}
  maxn1 = 5; {maximum number of unknows}
  {constants for intrank}
  maxm2 = maxn1 + 1; {maximum m for intrank}
  maxn2 = maxm1 + 1; {maximum n for intrank}
  maxe  = maxn1; {maximum e for intrank}
type
  {types for solveinteger}
  T1  = array [1..maxm1, 1..maxn1] of integer; {matrix A}
  T2  = array [1..maxm1] of integer; {vector b}
  T3  = array [1..maxn1] of integer; {solution x}
  T4  = array [1..maxn1, 1..maxn1] of integer; {auxiliary solutions Y}
  {types for intrank}
  T5 = array [1..maxm2, 1..maxn2 + maxe] of integer;
  T6 = array [1..maxm2] of integer;

function intrank(var mat : T5;
                     m : integer;
                     n : integer;
                     e : integer) : integer;
label
  NEXTROW, NEXTCOL, CONTINUE, OUT;
var
  i, j, k, Q, T, topel, nextel, itop, inext : integer;
  a                                         : T6;

  procedure findnext;
  var
    k    :  integer;
  begin
    nextel := 0;
    for k  := i to m do
      if (a[k] > nextel) and (k <> itop) then
      begin
        nextel := a[k];
        inext := k
      end
  end;

  procedure swaprows;
  var
    k    :  integer;
  begin
    for k := j to T do
    begin
      Q := -mat[i, k];
      mat[i, k] := mat[itop, k];
      mat[itop, k] := Q
    end;
    a[i] := itop
  end;

  procedure reduce;
  var
    k    :  integer;
  begin
    Q := mat[itop, j] div mat[inext, j];
    for k := j to T do
      mat[itop, k] := mat[itop, k] - Q * mat[inext, k];
    if mat[itop, j] < 0 then
      a[itop] := -mat[itop, j]
    else
      a[itop] := mat[itop, j]
  end;

begin
  i := 0;
  j := 0;
  itop := 0;
  T := n + e;
  NEXTROW:
          if itop <> i then
            swaprows;
  i := i + 1;
  if i > m then goto OUT;
  NEXTCOL:
          j := j + 1;
  if j > n then goto OUT;
  for k := i to m do
    if mat[k, j] < 0 then
      a[k] := -mat[k, j]
    else
      a[k] := mat[k, j];
  itop := i - 1;
  findnext;
  if nextel = 0 then goto NEXTCOL;
  CONTINUE:
           itop := inext;
  topel := nextel;
  findnext;
  if nextel = 0 then goto NEXTROW;
  reduce;
  goto CONTINUE;
  OUT:
      intrank := i - 1
end;

function solveinteger(    m : integer;
                          n : integer;
                      var A : T1;
                      var b : T2;
                      var d : integer;
                      var x : T3;
                      var Y : T4;
                      var k : integer) : boolean;
label
  OUT;
var
  i, j, rank, s : integer;
  mat           : T5;
begin
  for j := 1 to m do
  begin
    mat[1, j] := -b[j];
    for i := 1 to n do
      mat[i + 1, j] := A[j, i]
  end;
  for j := 1 to n + 1 do
    for i := 1 to n + 1 do
      if i = j then
        mat[i, j + m] := 1
      else
        mat[i, j + m] := 0;
  rank := intrank(mat, n + 1, m + 1, n);
  d := mat[rank, m + 1];
  if d = 0 then
  begin
    solveinteger := false;
    goto OUT
  end;
  for i := rank to m do
    if mat[rank, i] <> 0 then
    begin
      solveinteger := false;
      goto OUT
    end;
  solveinteger := true;
  if d < 0 then
    s := -1
  else
    s := 1;
  d := s * d;
  k := n - rank + 1;
  for i := 1 to n do
  begin
    x[i] := mat[rank, m + i + 1] * s;
    for j := 1 to k do
      Y[j, i] := mat[rank + j, m + i + 1]
  end;
  OUT:
end;

var
  m    : integer;
  n    : integer;
  A    : T1;
  b    : T2;
  d    : integer;
  x    : T3;
  Y    : T4;
  k    : integer;
  i, j : integer;

begin
  m := 2;
  n := 4;
  A[1, 1] := 10;
  A[1, 2] := 3;
  A[1, 3] := 3;
  A[1, 4] := 8;
  A[2, 1] := 6;
  A[2, 2] := -7;
  A[2, 3] := 0;
  A[2, 4] := -5;
  b[1] := 1;
  b[2] := 2;
  if (solveinteger(m, n, A, b, d, x, Y, k)) then
  begin
    writeln('d = ', d);
    writeln('k = ', k);
    writeln('x:');
    for j := 1 to n do
      write(' ', x[j]:4);
    writeln;
    writeln('Y:');
    for i := 1 to k do
    begin
      for j := 1 to n do
        write(' ', Y[i, j]:4);
      writeln
    end
  end
end.

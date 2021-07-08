prologues:=3;
verbatimtex %&latex
  \documentclass[a4paper,11pt]{report}
  \begin{document}
  etex

beginfig(1);
  u = 1cm;

  % The big ellipse is (x / 2)^2 + (y / (1 / 2))^2 = 1.
  % The small ellipse is (x / 2)^2 + (y / (3 / 2))^2 = 1.

  draw fullcircle xscaled 4u yscaled 1u;
  fill fullcircle xscaled 4u yscaled 1u withcolor 0.8white;
  draw fullcircle xscaled 4u yscaled 3u;

  drawarrow (0, 0) .. (-2u, -1.5u);
  label.llft(btex $X$ etex, (-2u, -1.5u));
  drawarrow (0, 0) .. (2.5u, 0);
  label.rt(btex $Y$ etex, (2.5u, 0));
  drawarrow (0, 0) .. (0, 2u);
  label.top(btex $Z$ etex, (0, 2u));
  label.lrt(btex $a$ etex, (-0.632u, -0.474u));
  label.urt(btex $a$ etex, (2u, 0));
  label.urt(btex $b$ etex, (0, 1.5u));

  pair P[];
  P[0] = (1u, 1u);
  P[2] = (1.5u, -sqrt(0.109375) * u);
  P[1] = (0.25u, 0.25 * (ypart P[2] / 1.5));

  draw (0, 1.5u) .. P[0] .. P[2] dashed evenly;
  draw P[1] -- P[0] dashed evenly;
  draw (0, 0) -- P[2] dashed evenly;

  dotlabel.rt(btex $P$ etex, P[0]);

  draw (0.5u, 0.5 * (ypart P[2] / 1.5))
    {dir 225} .. (-0.333u, (0.75 * (-0.333)) * u);
  label.top(btex $\lambda$ etex, (0, -0.4u));

  draw (0.75u, 0.64829u) {dir -45} .. (0.75u, -0.16536u);
  label.lft(btex $\varphi$ etex, (0.9u, 0.2u));
endfig;

beginfig(2);
  drawarrow (0, 0) .. (6u, 0);
  label.rt(btex $X$ etex, (6u, 0));
  drawarrow (0, 0) .. (0, 4u);
  label.top(btex $Z$ etex, (0, 4u));

  % The ellipse is (x / 5)^2 + (z / 3)^2 = 1.
  label.bot(btex $a$ etex, (5u, 0));
  label.lft(btex $b$ etex, (0, 3u));
  draw (0, 3u)
    for i = 1 upto 5: .. (i, 3 * sqrt(1 - (i * i) / 25)) * u endfor;

  numeric a, b, c, r;
  a := 2.7393 * u; % z_0
  b := 2.0388 * u; % x_0
  c := 1.3049 * u; % x_1

  draw (c, 0) -- (b, a);
  dotlabel.bot(btex $x_1$ etex, (c, 0));
  draw (b, 0) -- (b, a) dashed evenly;
  dotlabel.bot(btex $x_0$ etex, (b, 0));
  draw (0, a) -- (b, a) dashed evenly;
  dotlabel.lft(btex $z_0$ etex, (0, a));
  dotlabel.urt(btex $P'$ etex, (b, a));

  r = 0.5u;
  draw (r + c, 0)
    for i = 1 upto 5: .. (c + r * cosd(15 * i), r * sind(15 * i)) endfor;
  dotlabel.urt(btex $\varphi$ etex, (c, 0));
endfig;
end

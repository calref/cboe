rback=int(u[1].p{i,j}.r*255);
gback=int(u[1].p{i,j}.g*255);
bback=int(u[1].p{i,j}.b*255);

i==21&&j==8 ? debug(rback) : 1;
i==21&&j==8 ? debug(gback) : 1;
i==21&&j==8 ? debug(bback) : 1;

rmain=int(u[2].p{i,j}.r*255);
gmain=int(u[2].p{i,j}.g*255);
bmain=int(u[2].p{i,j}.b*255);

i==21&&j==8 ? debug(rmain) : 1;
i==21&&j==8 ? debug(gmain) : 1;
i==21&&j==8 ? debug(bmain) : 1;

rdiff=rmain - rback;
gdiff=gmain - gback;
bdiff=bmain - bback;

i==21&&j==8 ? debug(rdiff) : 1;
i==21&&j==8 ? debug(gdiff) : 1;
i==21&&j==8 ? debug(bdiff) : 1;

rgoal=rdiff > 0 ? 255 : 0;
ggoal=gdiff > 0 ? 255 : 0;
bgoal=bdiff > 0 ? 255 : 0;

i==21&&j==8 ? debug(rgoal) : 1;
i==21&&j==8 ? debug(ggoal) : 1;
i==21&&j==8 ? debug(bgoal) : 1;

rgd=rgoal - rback;
ggd=ggoal - gback;
bgd=bgoal - bback;

i==21&&j==8 ? debug(rgd) : 1;
i==21&&j==8 ? debug(ggd) : 1;
i==21&&j==8 ? debug(bgd) : 1;

ro=rgd == 0 ? 0 : rdiff / rgd;
bo=bgd == 0 ? 0 : bdiff / bgd;
go=ggd == 0 ? 0 : gdiff / ggd;

i==21&&j==8 ? debug(ro) : 1;
i==21&&j==8 ? debug(go) : 1;
i==21&&j==8 ? debug(bo) : 1;

max(ro,max(go,bo))
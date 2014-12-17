aval=u[1]*255;
bval=u[2]*255;

i==21&&j==8 ? debug(aval) : 1;
i==21&&j==8 ? debug(bval) : 1;

dist=bval-aval;

i==21&&j==8 ? debug(dist) : 1;

op=u[0].p{i,j}.a;

i==21&&j==8 ? debug(op) : 1;

mdist=op==0 ? 0 : dist/op;

i==21&&j==8 ? debug(mdist) : 1;

mdist/255
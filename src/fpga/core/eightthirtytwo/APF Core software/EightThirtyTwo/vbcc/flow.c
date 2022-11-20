/*  $VER: vbcc (flow.c) $Revision: 1.8 $    */
/*  Generierung des FLussgraphs und Optimierungen des Kontrollflusses   */

#include "opt.h"

static char FILE_[]=__FILE__;

int bvcmp(bvtype *dest,bvtype *src,size_t len)
/*  vergleicht zwei Bitvektoren    */
{
  len/=sizeof(bvtype);
  for(;len>0;len--)
    if(*dest++!=*src++) return(0);
  return(1);
}
/* Schnittmenge nicht leer? */
int bvdointersect(bvtype *p1,bvtype *p2,size_t len)
{
  len/=sizeof(bvtype);
  for(;len>0;len--)
    if(*p1++&*p2++)
      return 1;
  return 0;
}
void bvunite(bvtype *dest,bvtype *src,size_t len)
/*  berechnet Vereinigung zweier Bitvektoren    */
{
  len/=sizeof(bvtype);
  for(;len>0;len--)
    *dest++|=*src++;
}
void bvintersect(bvtype *dest,bvtype *src,size_t len)
/*  berechnet Durchschnitt zweier Bitvektoren   */
{
  len/=sizeof(bvtype);
  for(;len>0;len--)
    *dest++&=*src++;
}
void bvdiff(bvtype *dest,bvtype *src,size_t len)
/*  berechnet 'Differenz' zweier Bitvektoren    */
{
  len/=sizeof(bvtype);
  for(;len>0;len--)
    *dest++&=~(*src++);
}

unsigned int basic_blocks;

flowgraph *new_flowgraph(void)
{
  flowgraph *new;
  new=mymalloc(sizeof(*new));
  new->av_in=new->av_out=new->av_gen=new->av_kill=0;
  new->rd_in=new->rd_out=new->rd_gen=new->rd_kill=0;
  new->ae_in=new->ae_out=new->ae_gen=new->ae_kill=0;
  new->cp_in=new->cp_out=new->cp_gen=new->cp_kill=0;
  new->pt=0;  
  new->start=new->end=0;
  new->normalout=new->branchout=0;
  new->in=0;
  new->loopend=0;
#if ALEX_REG
  new->loop_depth=0;
#endif
  return new;
}

flowgraph *construct_flowgraph(void)
/*  entfernt ueberfluessige Labels und erzeugt Flussgraph   */
{
    IC *p,*cl;
    int firstl,lcnt,currentl,i,code,l;
    int *iseq,*used;
    flowgraph **lg,*g,*fg;
    if(cross_module){
      int lastl;
      firstl=0;lastl=0;lcnt=0;
      for(p=first_ic;p;p=p->next){
	if(p->code==LABEL){
	  if(p->typf<firstl) firstl=p->typf;
	  if(p->typf>lastl) lastl=p->typf;
	}
      }
      lcnt=lastl-firstl+1;
    }else{
      firstl=lastlabel;
      lcnt=label-firstl;
    }
    iseq=mymalloc(lcnt*sizeof(int));
    used=mymalloc(lcnt*sizeof(int));
    lg=mymalloc(lcnt*sizeof(flowgraph *));
    g=new_flowgraph();
    fg=g;
    g->start=first_ic;g->in=0;g->branchout=0;g->loopend=0;

    for(i=0;i<lcnt;i++) {iseq[i]=used[i]=0;lg[i]=0;}
    currentl=0;firstl++;
    /*  Diese Schleife entfernt alle Labels, die mit anderen    */
    /*  uebereinstimmen, merkt sich das und kennzeichnet alle   */
    /*  Labels, die benutzt werden.                             */
    /*  Ausserdem wird der Flussgraph teilweise aufgebaut.      */
    if(DEBUG&1024) {puts("construct_flowgraph(): loop1");/*scanf("%d",&i);*/}
    i=1;g->index=i;
    for(p=first_ic;p;){
        code=p->code;
        if(code>=BEQ&&code<=BRA){
            l=p->typf;
            /*  als used markieren; falls aequivalent, das erste markieren  */
            if(iseq[l-firstl]) used[iseq[l-firstl]-firstl]=1;
                else           used[l-firstl]=1;
            /*  Flussgraph beenden und evtl. naechsten Knoten erzeugen  */
            g->end=p;
            if(p->next){
	      g->normalout=new_flowgraph();
	      g->normalout->in=mymalloc(sizeof(flowlist));
	      g->normalout->in->next=0;
	      g->normalout->in->graph=g;
	      g=g->normalout;
	      g->start=p->next;
	      g->branchout=0;
	      g->loopend=0;
	      g->index=++i;
            }else g->normalout=0;

            currentl=0;p=p->next;continue;
        }
        if(code==ALLOCREG||code==FREEREG){p=p->next; continue;}
        if(code!=LABEL){currentl=0;p=p->next;continue;}
        /*  ist ein Label   */
        l=p->typf;
        if(currentl){
	    IC *m;
            iseq[l-firstl]=currentl;
	    if(l==return_label) return_label=currentl;
            if(used[l-firstl]) used[currentl-firstl]=1;
	    cl->flags|=p->flags;
	    m=p;p=p->next;
            remove_IC(m);
	    continue;
/*            if(DEBUG&1024) printf("label %d==%d\n",l,iseq[l-firstl]);*/
        }else{
            currentl=l;cl=p;
            if(g->start!=p){
                g->end=p->prev;
                g->normalout=new_flowgraph();
                g->normalout->in=mymalloc(sizeof(flowlist));
                g->normalout->in->next=0;
                g->normalout->in->graph=g;
                g=g->normalout;
                g->start=p;
                g->branchout=0;
                g->loopend=0;
                g->index=++i;
            }else g->branchout=0;
            lg[l-firstl]=g;
        }
	p=p->next;
    }
    g->end=last_ic;g->normalout=g->branchout=0;
    if(DEBUG&(16384|1024)) printf("%d basic blocks\n",i);
    basic_blocks=i;

/*    if(DEBUG&1024) for(i=firstl;i<=lcnt;i++) printf("L%d used: %d\n",i,used[i-firstl]);*/
    /*  Diese Schleife entfernt alle nicht benutzten Labels und biegt alle  */
    /*  Branches auf aequivalente Labels um.                                */
    if(DEBUG&1024) {puts("construct_flowgraph(): loop2");/*scanf("%d",&i);*/}
    g=fg;
    while(g){
        int flag=0;flowlist *lp;
/*        printf("g=%p\n",(void *)g);*/
        g->av_in=g->av_out=g->av_gen=g->av_kill=0;
        g->rd_in=g->rd_out=g->rd_gen=g->rd_kill=0;
        g->ae_in=g->ae_out=g->ae_gen=g->ae_kill=0;
        g->cp_in=g->cp_out=g->cp_gen=g->cp_kill=0;
	g->pt=0;
        p=g->start;
        while(p&&!flag){
/*            pric2(stdout,p);*/
            code=p->code;
            if(code>=BEQ&&code<=BRA){
                l=p->typf;
                if(iseq[l-firstl]) p->typf=l=iseq[l-firstl];
                /*  in Flussgraph eintragen */
                g->branchout=lg[l-firstl];
                if(!lg[l-firstl]) error(353);
                lp=lg[l-firstl]->in;
                /*  das hier sollte man noch schoener machen    */
                if(!lp){
                    lg[l-firstl]->in=mymalloc(sizeof(flowlist));
                    lg[l-firstl]->in->next=0;
                    lg[l-firstl]->in->graph=g;
                }else{
                    while(lp&&lp->next) lp=lp->next;
                    lp->next=mymalloc(sizeof(flowlist));
                    lp->next->next=0;
                    lp->next->graph=g;
                }
            }
/*            if(code==LABEL&&!used[p->typf-firstl]) remove_IC(p);*/
            if(p==g->end) flag=1;
            p=p->next;
        }
        g=g->normalout;
    }
    /*  Unbenutzte Labels entfernen und Bloecke verbinden   */
    if(DEBUG&1024) {puts("construct_flowgraph(): loop3");/*scanf("%d",&i);*/}
    for(g=fg;g;g=g->normalout){
        if(g->end&&(g->end->code<BEQ||g->end->code>BRA)){
            flowgraph *next=g->normalout;flowlist *lp;
            if(next&&next->start&&next->start->code==LABEL&&!used[next->start->typf-firstl]){
                if(next->end!=next->start) g->end=next->end;
                g->normalout=next->normalout;
                g->branchout=next->branchout;
                free(next->in); /*  darf eigentlich nur einen Vorgaenger haben  */
                /*  in im Nachfolgeknoten auf den ersten der beiden setzen  */
                if(next->normalout&&next->normalout->in) next->normalout->in->graph=g;
                /*  in im Ziel von next->branchout auf den ersten setzen    */
                if(next->branchout){
                    lp=next->branchout->in;
                    while(1){
                        if(lp->graph==next){ lp->graph=g;break;}
                        lp=lp->next;if(!lp) ierror(0);
                    }
                }
                if(DEBUG&1024){ printf("unused label deleted:\n");pric2(stdout,next->start);}
                remove_IC(next->start);
                free(next);
            }
        }
        /*  unbenutzte Labels entfernen */
        if(g->start&&g->start->code==LABEL&&!used[g->start->typf-firstl])
            remove_IC_fg(g,g->start);
    }
    free(iseq);
    free(used);
    return(fg);
}

void print_flowgraph(flowgraph *g)
/*  Gibt Flussgraph auf Bildschirm aus  */
{
    static int dontprint=0;
    int flag,i;flowlist *lp;IC *ip;
    if(dontprint>0) {dontprint--;return;}
    if(dontprint!=-1){
      puts("print_flowgraph()");scanf("%d",&i);
      if(i<-1){dontprint=-i;return;}
      if(!i) return;
      if(i==-1){dontprint=-1; i=1;}
    }else{
      i=1;
    }
    while(g){
        printf("\nBasic Block nr. %d\n",g->index);
        printf("\tin from ");
        lp=g->in;
        while(lp){if(lp->graph) printf("%d ",lp->graph->index);lp=lp->next;}
        printf("\n\tout to %d %d\n",g->normalout?g->normalout->index:0,g->branchout?g->branchout->index:0);
        if(g->loopend) printf("head of a loop ending at block %d\n",g->loopend->index);
	if(!(optflags&32)&&g->pt)
	  ierror(0);
        if(i&2){
            printf("av_gen:\n"); print_av(g->av_gen);
            printf("av_kill:\n"); print_av(g->av_kill);
            printf("av_in:\n"); print_av(g->av_in);
            printf("av_out:\n"); print_av(g->av_out);
        }
        if(i&4){
            printf("rd_gen:\n"); print_rd(g->rd_gen);
            printf("rd_kill:\n"); print_rd(g->rd_kill);
            printf("rd_in:\n"); print_rd(g->rd_in);
            printf("rd_out:\n"); print_rd(g->rd_out);
        }
        if(i&8){
            printf("ae_gen:\n"); print_ae(g->ae_gen);
            printf("ae_kill:\n"); print_ae(g->ae_kill);
            printf("ae_in:\n"); print_ae(g->ae_in);
            printf("ae_out:\n"); print_ae(g->ae_out);
        }
        if(i&16){
            printf("cp_gen:\n"); print_cp(g->cp_gen);
            printf("cp_kill:\n"); print_cp(g->cp_kill);
            printf("cp_in:\n"); print_cp(g->cp_in);
            printf("cp_out:\n"); print_cp(g->cp_out);
        }
        if(i&32){
            int r;
            for(r=1;r<=MAXR;r++)
                if(g->regv[r]) printf("(%s),%ld assigned to %s\n",g->regv[r]->identifier,(long)zm2l(g->regv[r]->offset),regnames[r]);
        }
	if(i&64){
	  print_pt(g->pt);
	}
        flag=0;ip=g->start;
        while(ip&&!flag){
            pric2(stdout,ip);
            if(i&64){
                int r;
                printf("changes: ");
                for(r=0;r<ip->change_cnt;r++)
                    printf("(%s,%ld,%d,%d)",ip->change_list[r].v->identifier,(long)zm2l(ip->change_list[r].v->offset),ip->change_list[r].flags,ip->change_list[r].v->index);
                printf("\nuses: ");
                for(r=0;r<ip->use_cnt;r++)
                    printf("(%s,%ld,%d,%d)",ip->use_list[r].v->identifier,(long)zm2l(ip->use_list[r].v->offset),ip->use_list[r].flags,ip->use_list[r].v->index);
                printf("\n");
            }
            if(ip==g->end) flag=1;
            ip=ip->next;
        }
        g=g->normalout;
    }
}
void free_flowgraph(flowgraph *g)
/*  Gibt Flussgraph frei    */
{
    flowgraph *pm;flowlist *lp,*lpm;
    if(DEBUG&(16384|1024)) puts("free_flowgraph()");
    while(g){
        lp=g->in;
        while(lp){
            lpm=lp->next;
            free(lp);
            lp=lpm;
        }
        free(g->av_in);
        free(g->av_out);
        free(g->av_gen);
        free(g->av_kill);
        free(g->rd_in);
        free(g->rd_out);
        free(g->rd_gen);
        free(g->rd_kill);
        free(g->ae_in);
        free(g->ae_out);
        free(g->ae_gen);
        free(g->ae_kill);
        free(g->cp_in);
        free(g->cp_out);
        free(g->cp_gen);
        free(g->cp_kill);
	free_pt(g->pt);

        pm=g->normalout;
        free(g);
        g=pm;
    }
}
static void mark_reachable(flowgraph *fg)
/* negiert den index aller Bloecke, die reachable sind */
{
  fg->index=-fg->index;
  if(fg->branchout&&fg->branchout->index>=0)
    mark_reachable(fg->branchout);
  if(fg->normalout&&(!fg->end||fg->end->code!=BRA)&&fg->normalout->index>=0)
    mark_reachable(fg->normalout);
}
flowgraph *jump_optimization(void)
/*  entfernt ueberfluessige Spruenge etc.                           */
{
    flowgraph *fg,*g;IC *p;int changed,i;
    flowlist *lp;
    do{
        changed=0;
        fg=construct_flowgraph();
	mark_reachable(fg);
        if(DEBUG&1024) {printf("jump_optimization() pass\n");print_flowgraph(fg);}
        g=fg;
        while(g){
            /*  tote Bloecke entfernen                  */
	    if(g->index<0){
	        g->index=-g->index;
	    }else{
                IC *m;
                if(DEBUG&1024) printf("deleting dead block %d\n",g->index);
#ifdef HAVE_MISRA
/* removed */
#endif
                p=g->start;i=0;
                while(p&&!i){
                    if(p==g->end) i=1;
                    if(DEBUG&1024) pric2(stdout,p);
                    m=p->next;
                    remove_IC_fg(g,p);changed=gchanged=1;
                    p=m;
                }
                if(g->branchout){
                /*  Eintrag in Ziel loeschen (nur einmal, falls auch normalout)    */
                    lp=g->branchout->in;
                    while(lp){
                        if(lp->graph==g){ lp->graph=0;break;}
                        lp=lp->next;
                    }
                    g->branchout=0;
                }
                g=g->normalout;continue;
            }
            /*  Spruenge zum folgenden Code entfernen   */
            if(g->normalout&&g->normalout==g->branchout){
                p=g->end;
                if(!p||p->code<BEQ||p->code>BRA) ierror(0);
                if(DEBUG&1024){printf("branch to following label deleted:\n");pric2(stdout,p);}
                remove_IC_fg(g,p);g->branchout=0;changed=gchanged=1;
                p=g->end;
                /*  vorangehenden Vergleich auch entfernen  */
                if(p&&(p->code==COMPARE||p->code==TEST)){
                    if(DEBUG&1024){printf("preceding comparison also deleted:\n");pric2(stdout,p);}
                    remove_IC_fg(g,p);
                }
            }

            /*  bcc l1;bra l2;l1 aendern    */
            p=g->end;
            if(p&&p->code>=BEQ&&p->code<BRA&&g->normalout){
                if(g->normalout->start&&g->normalout->start->code==BRA){
                    if(g->normalout->normalout==g->branchout){
                        g->branchout=g->normalout->branchout;
                        i=p->typf;
                        p->typf=g->normalout->start->typf;
                        if(DEBUG&1024) printf("changing bcc l%d;bra l%d;l%d to b!cc l%d\n",i,p->typf,i,p->typf);
                        switch(p->code){
                        case BEQ: p->code=BNE;break;
                        case BNE: p->code=BEQ;break;
                        case BLT: p->code=BGE;break;
                        case BGE: p->code=BLT;break;
                        case BGT: p->code=BLE;break;
                        case BLE: p->code=BGT;break;
                        }
                        g->normalout->branchout=g->normalout->normalout;
                        g->normalout->start->typf=i;
                        changed=gchanged=1;
                    }
                }
            }
            /*  Haben alle Vorgaenger eines Blocks die selbe Anweisung am   */
            /*  Blockende und keinen weiteren Nachfolger, dann kann die     */
            /*  Anweisung in den Nachfogerblock geschoben werden            */
            i=0;p=0;
            for(lp=g->in;lp;lp=lp->next){
                if(lp->graph){
                    IC *np;
                    flowgraph *ng=lp->graph;
                    flowlist *l2;
                    /*  doppelte Bloecke loeschen und ueberspringen */
                    for(l2=g->in;l2;l2=l2->next)
                        if(l2!=lp&&l2->graph==ng) break;
                    if(l2){ lp->graph=0;continue;}
                    np=ng->end;
                    if(!np){ i=-1;break;}
                    if(ng->branchout&&(np->code!=BRA||ng->branchout!=g)){i=-1;break;}
                    if(np->code==BRA) np=np->prev;
                    if(!np){ i=-1;break;}
                    if(!p){
                        i=1;
                        p=np;
                    }else{
                        if(p->code==np->code&&p->typf==np->typf&&
                           p->code!=CALL&&p->code!=GETRETURN&&p->code!=PUSH&&(p->code<TEST||p->code>COMPARE)&&
                           !compare_objs(&p->q1,&np->q1,p->typf)&&
                           !compare_objs(&p->q2,&np->q2,p->typf)&&
                           !compare_objs(&p->z,&np->z,p->typf)){
                            i++;
                        }else{
                            i=-1;
                            break;
                        }
                    }
                }
            }
            if(i>1&&g->start){
                IC *new=new_IC();
                if(DEBUG&1024){ printf("moving instruction from preceding blocks to successor:\n");pric2(stdout,p);}
                changed=gchanged=1;
                memcpy(new,p,ICS);
                new->use_cnt=new->change_cnt=0;
                new->use_list=new->change_list=0;
                if(g->start->code==LABEL){
                    insert_IC_fg(g,g->start,new);
                }else{
                    insert_IC_fg(g,g->start->prev,new);
                }
                for(lp=g->in;lp;lp=lp->next){
                    flowgraph *ng=lp->graph;
                    if(ng){
                        if(!ng->end) ierror(0);
                        if(ng->end->code==BRA){
                            remove_IC_fg(ng,ng->end->prev);
                        }else{
                            remove_IC_fg(ng,ng->end);
                        }
                    }
                }
            }
            /*  Haben alle Nachfolger eines Blocks die selbe Anweisung am   */
            /*  Blockbeginn und keinen weiteren Vorgaenger, dann kann die   */
            /*  Anweisung in den Vorgaengerblock geschoben werden           */
            if(g->branchout&&g->normalout&&g->branchout!=g->normalout&&g->end&&g->end->code!=BRA){
                flowgraph *a=g->normalout,*b=g->branchout;
                IC *as=a->start,*bs=b->start,*tp;
                int destroys;
                if(as&&as->code==LABEL&&as!=a->end) as=as->next;
                if(bs&&bs->code==LABEL&&bs!=b->end) bs=bs->next;

                if(as&&bs&&as->code==bs->code&&as->code!=PUSH&&(as->code<TEST||as->code>COMPARE)&&as->typf==bs->typf&&
                   !compare_objs(&as->q1,&bs->q1,as->typf)&&
                   !compare_objs(&as->q2,&bs->q2,as->typf)&&
                   !compare_objs(&as->z,&bs->z,as->typf)){
                    i=0;
                    for(lp=a->in;lp;lp=lp->next)
                        if(lp->graph&&lp->graph!=g&&(lp->graph->branchout==a||!lp->graph->end||lp->graph->end->code!=BRA)) i=1;
                    for(lp=b->in;lp;lp=lp->next)
                        if(lp->graph&&lp->graph!=g&&(lp->graph->branchout==b||!lp->graph->end||lp->graph->end->code!=BRA)) i=1;
                    if(!i){
                        if(!(tp=g->end->prev)) ierror(0);
                        if(tp->code!=TEST&&tp->code!=COMPARE)
                            ierror(0);
                        /*  schauen, ob die Anweisung eine evtl. TEST   */
                        /*  oder COMPARE-Anweisung beeinflusst          */
                        destroys=0;
                        if(as->z.flags&DREFOBJ) destroys|=1;
                        if(as->code==CALL) destroys|=2;
                        if(tp->q1.flags&VAR){
                            if(destroys&3){
                                if((tp->q1.v->flags&USEDASADR)||
                                   (tp->q1.flags&DREFOBJ)||
                                   (tp->q1.v->storage_class==EXTERN)||
                                   (tp->q1.v->nesting==0))
                                    i=1;
                                if((destroys&2)&&tp->q1.v->storage_class==STATIC)
                                    i=1;
                            }
                            if((as->z.flags&VAR)&&as->z.v==tp->q1.v)
                                    i=1;
                        }
                        if(tp->q2.flags&VAR){
                            if(destroys&3){
                                if((tp->q2.v->flags&USEDASADR)||
                                   (tp->q2.flags&DREFOBJ)||
                                   (tp->q2.v->storage_class==EXTERN)||
                                   (tp->q2.v->nesting==0))
                                    i=1;
                                if((destroys&2)&&tp->q2.v->storage_class==STATIC)
                                    i=1;
                            }
                            if((as->z.flags&VAR)&&as->z.v==tp->q2.v)
                                i=1;
                        }
                        if(!i&&!(disable&2)){
                            if(DEBUG&1024){ printf("moving instruction from following blocks to predecessor:\n");pric2(stdout,as);}
                            p=new_IC();
                            memcpy(p,as,ICS);
                            remove_IC_fg(a,as);
                            remove_IC_fg(b,bs);
                            p->use_cnt=p->change_cnt=0;
                            p->use_list=p->change_list=0;
                            insert_IC_fg(g,g->end->prev->prev,p);
                            changed=gchanged=1;
                        }
                    }
                }
            }
            g=g->normalout;
        }
        g=fg;
        while(g){
            /*  Spruenge zu Spruengen umsetzen; einige Zeiger im Flussgraph */
            /*  werden nicht korrekt aktualisiert, aber das sollte egal sein*/
            p=g->start;
            for(i=0;i<2;i++){
                if(i){if(p&&p->code==LABEL) p=p->next; else break;}
                if(p&&p->code>=BEQ&&p->code<=BRA){
                    lp=g->in;
                    while(lp){
                        if(lp->graph&&lp->graph->branchout==g&&(/*lp->graph->end->code==p->code||*/p->code==BRA)&&lp->graph->end->typf!=p->typf){
                            if(DEBUG&1024){printf("branch bypassed to L%d:\n",p->typf);pric2(stdout,lp->graph->end);}
                            if(lp->graph->end->code<BEQ||lp->graph->end->code>BRA) ierror(0);
                            lp->graph->branchout=g->branchout;
                            lp->graph->end->typf=p->typf;changed=gchanged=1;
                        }
                        lp=lp->next;
                    }
                }
            }


	    g=g->normalout;
	}
        if(changed) free_flowgraph(fg);
    }while(changed);
    return fg;
}

void insert_IC_fg(flowgraph *fg,IC *p,IC *new)
/*  fuegt ein IC hinter p ein unter Beibehaltung des Flussgraphen   */
{
    if(fg->start){
        if(!p||p==fg->start->prev) fg->start=new;
        if(p==fg->end) fg->end=new;
    }else{
        fg->start=fg->end=new;
    }
    insert_IC(p,new);
}

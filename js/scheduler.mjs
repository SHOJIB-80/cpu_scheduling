/** Faithful browser port of original/cpu_scheduler.c's six scheduling functions.
 * Equal-key ties keep original input order for FCFS/SJF/SRTF/Priority.
 * RR enqueues newly arrived processes in input order before requeuing the current one.
 * RR + Priority picks lowest priority number from waiting queue; ties use queue order.
 * Fixes original fixed-size queue/bar overflows and rejects invalid inputs.
 */
export const ALGORITHMS = Object.freeze([
  {id:'fcfs', name:'First Come, First Served', short:'FCFS', type:'Non-preemptive'},
  {id:'sjf', name:'Shortest Job First', short:'SJF', type:'Non-preemptive'},
  {id:'srtf', name:'Shortest Remaining Time First', short:'SRTF', type:'Preemptive'},
  {id:'rr', name:'Round Robin', short:'RR', type:'Preemptive'},
  {id:'priority', name:'Priority Scheduling', short:'Priority', type:'Non-preemptive'},
  {id:'rrPriority', name:'Round Robin + Priority', short:'RR + Priority', type:'Preemptive'}
]);
export function validateProcesses(data, quantum = 2) {
  if (!Array.isArray(data) || data.length < 1 || data.length > 30) throw Error('Enter between 1 and 30 processes.');
  const ids = new Set();
  for (const [index, p] of data.entries()) {
    for (const key of ['pid','at','bt','pr']) if (!Number.isSafeInteger(p[key])) throw Error(`Process ${index+1}: ${key.toUpperCase()} must be a whole number.`);
    if (p.pid < 1 || p.pid > 9999) throw Error(`Process ${index+1}: ID must be 1–9999.`);
    if (ids.has(p.pid)) throw Error(`Duplicate process ID: P${p.pid}.`);
    ids.add(p.pid);
    if (p.at < 0 || p.at > 10000) throw Error(`P${p.pid}: arrival must be between 0 and 10000.`);
    if (p.bt < 1 || p.bt > 500) throw Error(`P${p.pid}: burst must be between 1 and 500.`);
    if (p.pr < 0 || p.pr > 999) throw Error(`P${p.pid}: priority must be between 0 and 999.`);
  }
  if (!Number.isSafeInteger(quantum) || quantum < 1 || quantum > 500) throw Error('Time quantum must be a whole number between 1 and 500.');
  return true;
}
export function simulate(id, data, quantum=2) {
  if (!ALGORITHMS.some(a=>a.id===id)) throw Error('Unknown scheduling algorithm.');
  validateProcesses(data, quantum);
  // reset(): work on a fresh copy of the SAME original process set for each run.
  const p = data.map(x=>({...x, rem:x.bt, ct:0, tat:0, wt:0, rt:0, started:false}));
  const bars=[]; let time=0, done=0;
  function addBar(pid,start,end,merge=false){
    if(end<=start)return;
    const last=bars.at(-1);
    if(merge && last && last.pid===pid && last.end===start)last.end=end;
    else bars.push({pid,start,end});
  }
  function nextArrival(){return Math.min(...p.filter(x=>x.rem>0).map(x=>x.at));}
  function idleUntilNext(){const next=nextArrival(); if (!Number.isFinite(next)||next<=time)throw Error('Scheduler stalled.');addBar(-1,time,next);time=next;}
  function start(x){if(!x.started){x.started=true;x.rt=time-x.at;}}
  function complete(x){x.ct=time;x.tat=x.ct-x.at;x.wt=x.tat-x.bt;done++;}
  function runNonPreemptive(x){start(x);addBar(x.pid,time,time+x.bt);time+=x.bt;x.rem=0;complete(x);}
  if(id==='fcfs'){
    const order=p.map((_,i)=>i).sort((a,b)=>p[a].at-p[b].at || a-b);
    for(const idx of order){if(time<p[idx].at){addBar(-1,time,p[idx].at);time=p[idx].at;}runNonPreemptive(p[idx]);}
  }else if(id==='sjf'||id==='priority'){
    while(done<p.length){let best=-1;
      for(let i=0;i<p.length;i++)if(p[i].rem>0 && p[i].at<=time && (best===-1 || (id==='sjf'?p[i].bt<p[best].bt:p[i].pr<p[best].pr)))best=i;
      if(best<0){idleUntilNext();continue;}runNonPreemptive(p[best]);
    }
  }else if(id==='srtf'){
    while(done<p.length){let best=-1;
      for(let i=0;i<p.length;i++)if(p[i].rem>0 && p[i].at<=time && (best===-1||p[i].rem<p[best].rem))best=i;
      if(best<0){idleUntilNext();continue;}
      const x=p[best];start(x);addBar(x.pid,time,time+1,true);x.rem--;time++;if(x.rem===0)complete(x);
    }
  }else{
    // Same arrival/requeue and priority-pick behavior as the supplied C source,
    // but dynamically sized queue avoids its queue[50] buffer overflow.
    const queue=[], everQueued=new Set();
    function enqueueArrivals(){for(let i=0;i<p.length;i++)if(p[i].at<=time&&!everQueued.has(i)){queue.push(i);everQueued.add(i);}}
    enqueueArrivals();
    while(done<p.length){if(!queue.length){idleUntilNext();enqueueArrivals();continue;}
      let pick=0;
      if(id==='rrPriority')for(let i=1;i<queue.length;i++)if(p[queue[i]].pr<p[queue[pick]].pr)pick=i;
      const [idx]=queue.splice(pick,1),x=p[idx];start(x);
      const slice=Math.min(x.rem,quantum);addBar(x.pid,time,time+slice);time+=slice;x.rem-=slice;
      enqueueArrivals(); // arrival-first rule before current process is requeued
      if(x.rem===0)complete(x);else queue.push(idx);
    }
  }
  const total=time, busy=p.reduce((s,x)=>s+x.bt,0), sum=(key)=>p.reduce((s,x)=>s+x[key],0);
  if(bars[0]?.start!==0 || bars.at(-1)?.end!==total || bars.some((b,i)=>b.end<=b.start||(i&&bars[i-1].end!==b.start)))throw Error('Invalid Gantt chart generated.');
  if(p.some(x=>x.ct<x.at+x.bt||x.wt<0||x.rt<0))throw Error('Invalid process metrics generated.');
  return {id, name:ALGORITHMS.find(a=>a.id===id).name,short:ALGORITHMS.find(a=>a.id===id).short,
    processes:p.map(({pid,at,bt,pr,ct,tat,wt,rt})=>({pid,at,bt,pr,ct,tat,wt,rt})),
    bars, metrics:{avgWT:sum('wt')/p.length,avgTAT:sum('tat')/p.length,avgRT:sum('rt')/p.length,cpu:(busy/total)*100, total,busy, idle:total-busy, throughput:p.length/total}};
}
export function compare(ids, processes, quantum){if(!Array.isArray(ids)||!ids.length||new Set(ids).size!==ids.length)throw Error('Choose unique algorithms.');return ids.map(id=>simulate(id,processes,quantum));}

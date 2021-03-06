#!/bin/bash

RESULTDIR=result/

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi


# import params.
. ../params.sh
P=${SLURM_CPUS_ON_NODE}
NP=$(expr ${SLURM_NTASKS} / ${SLURM_CPUS_ON_NODE})


echo starting time is $(date)

for N in ${NUM_INT_NS} ;
do
   for INTEN in ${INTENSITIES} ;
   do
       TIMEFILE=${RESULTDIR}/mpi_advanced_manager_worker_${N}_${INTEN}_${NP}_${P}
       
       mpirun ${MPIRUN_PARAMS}  ./mpi_advanced_manager_worker 1 0 10 ${N} ${INTEN} > /dev/null 2> ${TIMEFILE}

       process_time_file ${TIMEFILE}
   done
done

echo ending time is $(date)

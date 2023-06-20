#include "headers/simulation_fh.h"

int simulation(output matrix[][15], double decessi[], int iteration, int finite, float probability){
    int currentJob[5]={0,0,0,0,0};
    int currentBatch[5]={0,0,0,0,0};

    nodeData triageStats;
    nodeData redCodeStats;
    nodeData traumaStats;
    nodeData medicalStats;
    nodeData minorStats;

    nodeData yellowTraumaStats;
    nodeData greenTraumaStats;

    nodeData yellowMedicalStats;
    nodeData greenMedicalStats;

    nodeData yellowMinorStats;
    nodeData greenMinorStats;
    nodeData whiteMinorStats;


    initOutputStats(&triageStats,SERVERSTRIAGE);
    initOutputStats(&redCodeStats,SERVERSRED);
    initOutputStats(&traumaStats,SERVERSTRAUMA);
    initOutputStats(&medicalStats,SERVERSMEDICAL);
    initOutputStats(&minorStats,SERVERSMINOR);
    //partial statistic
    initOutputStats(&yellowTraumaStats, SERVERSTRAUMA);
    initOutputStats(&greenTraumaStats, SERVERSTRAUMA);

    initOutputStats(&yellowMedicalStats, SERVERSMEDICAL);
    initOutputStats(&greenMedicalStats, SERVERSMEDICAL);

    initOutputStats(&yellowMinorStats, SERVERSMINOR);
    initOutputStats(&greenMinorStats, SERVERSMINOR);
    initOutputStats(&whiteMinorStats, SERVERSMINOR);

    intitParamService();

    arrival=START;

    // number of people in queue for a specific area (and code color)
    int triageNumber=0;
    int redNumber=0;

    int medicalYellowNumber=0;
    int medicalGreenNumber=0;

    int minorYellowNumber=0;
    int minorGreenNumber=0;
    int minorWhiteNumber=0;

    int traumaYellowNumber=0;
    int traumaGreenNumber=0;

    //sembra sensato tenere quanti ce ne sono in servizio per le statistiche
    int traumaInServiceYellow=0;
    int medicalInServiceYellow=0;
    int minorInServiceYellow=0;

    int traumaInServiceGreen=0;
    int medicalInServiceGreen=0;
    int minorInServiceGreen=0;

    int minorInServiceWhite=0;

    //PlantSeeds(SEED);
    event t;

    //an array of servers for each multiserver
    multiserver triage[SERVERSTRIAGE];
    multiserver redCode[SERVERSRED];
    multiserver trauma[SERVERSTRAUMA];
    multiserver medical[SERVERSMEDICAL];
    multiserver minor[SERVERSMINOR];

    initServerData(triage, SERVERSTRIAGE);
    initServerData(medical, SERVERSMEDICAL);
    initServerData(minor, SERVERSMINOR);
    initServerData(trauma, SERVERSTRAUMA);
    initServerData(redCode, SERVERSRED);
    int countOrange=0;
    t.arrival=getArrival();
    t.triageCompletion=-1;
    t.redCodeCompletion=-1;
    t.traumaCompletion=-1;
    t.medicalCompletion=-1;
    t.minorCompletion=-1;
    t.current=START;
    //t.last=START;   //per statistiche non serve TODO cancellare

    //@improved while condition
    while((finite && t.arrival<STOP) || (!finite && (condizioneBatchMeans(currentBatch,numBatch) || condizioneBatchMeans(currentJob,numJobInBatch))) ||
          (finite && triageNumber+redNumber+traumaYellowNumber+
                     traumaGreenNumber+minorYellowNumber+minorGreenNumber+
                     minorWhiteNumber+medicalYellowNumber+medicalGreenNumber)>0){
        t.next=getSpecificMin(t.arrival, triage, SERVERSTRIAGE);
        t.next=getSpecificMin(t.next, redCode, SERVERSRED);
        t.next=getSpecificMin(t.next, trauma, SERVERSTRAUMA);
        t.next=getSpecificMin(t.next, medical, SERVERSMEDICAL);
        t.next=getSpecificMin(t.next, minor, SERVERSMINOR);

        if((finite && triageNumber>0) || (!finite && triageNumber>0 && (currentBatch[0]<numBatch || currentJob[0]<numJobInBatch)))  statsUpdater(&triageStats,triageNumber,t.current,t.next);

        if((finite && redNumber>0) || (!finite && redNumber>0 && (currentBatch[1]<numBatch || currentJob[1]<numJobInBatch)))  statsUpdater(&redCodeStats,redNumber,t.current,t.next);

        if((finite && traumaYellowNumber+traumaGreenNumber>0) || (!finite && traumaYellowNumber+traumaGreenNumber>0 && (currentBatch[2]<numBatch || currentJob[2]<numJobInBatch)))  statsUpdater(&traumaStats,traumaYellowNumber+traumaGreenNumber,t.current,t.next);

        if((finite && medicalYellowNumber+medicalGreenNumber>0) || (!finite && medicalYellowNumber+medicalGreenNumber>0 && (currentBatch[4]<numBatch || currentJob[4]<numJobInBatch)))  statsUpdater(&medicalStats,medicalYellowNumber+medicalGreenNumber,t.current,t.next);

        if((finite && minorYellowNumber+minorGreenNumber+minorWhiteNumber>0) || (!finite && minorYellowNumber+minorGreenNumber+minorWhiteNumber>0 && (currentBatch[3]<numBatch || currentJob[3]<numJobInBatch)))  statsUpdater(&minorStats,minorYellowNumber+minorGreenNumber+minorWhiteNumber,t.current,t.next);

        if((finite && traumaYellowNumber>0) || (!finite && traumaYellowNumber>0 && (currentBatch[2]<numBatch || currentJob[2]<numJobInBatch))) partialStatsUpdater(&yellowTraumaStats, traumaYellowNumber, traumaInServiceYellow, t.current, t.next);

        if((finite && traumaGreenNumber>0) || (!finite && traumaGreenNumber>0 && (currentBatch[2]<numBatch || currentJob[2]<numJobInBatch))) partialStatsUpdater(&greenTraumaStats, traumaGreenNumber, traumaInServiceGreen, t.current, t.next);

        if((finite && medicalYellowNumber>0) || (!finite && medicalYellowNumber>0 && (currentBatch[4]<numBatch || currentJob[4]<numJobInBatch))) partialStatsUpdater(&yellowMedicalStats, medicalYellowNumber, medicalInServiceYellow, t.current, t.next);

        if((finite && medicalGreenNumber>0) || (!finite && medicalGreenNumber>0 && (currentBatch[4]<numBatch || currentJob[4]<numJobInBatch))) partialStatsUpdater(&greenMedicalStats, medicalGreenNumber, medicalInServiceGreen, t.current, t.next);

        if((finite && minorYellowNumber>0 )|| (!finite && minorYellowNumber>0 && (currentBatch[3]<numBatch || currentJob[3]<numJobInBatch))) partialStatsUpdater(&yellowMinorStats,minorYellowNumber, minorInServiceYellow, t.current,t.next);

        if((finite && minorGreenNumber>0) || (!finite && minorGreenNumber>0 && (currentBatch[3]<numBatch || currentJob[3]<numJobInBatch))) partialStatsUpdater(&greenMinorStats,minorGreenNumber,minorInServiceGreen,t.current,t.next);

        if((finite && minorWhiteNumber>0) || (!finite && minorWhiteNumber>0 && (currentBatch[3]<numBatch || currentJob[3]<numJobInBatch))) partialStatsUpdater(&whiteMinorStats,minorWhiteNumber,minorInServiceWhite,t.current,t.next);

        t.current=t.next;

        if(!finite)
        {
            if(numJobInBatch==currentJob[0] && currentBatch[0]<numBatch)
            {
                writeStats(matrix[currentBatch[0]],triageStats,0);
                initOutputStats(&triageStats,SERVERSTRIAGE);
                initTime(&triageStats,t.current);
                currentJob[0]=0;
                currentBatch[0]++;

            }
            if(numJobInBatch==currentJob[1]&& currentBatch[1]<numBatch)
            {

                writeStats(matrix[currentBatch[1]],redCodeStats,1);
                decessi[currentBatch[1]]=decessi[currentBatch[1]]/redCodeStats.index;
                initOutputStats(&redCodeStats,SERVERSRED);
                initTime(&redCodeStats,t.current);
                currentJob[1]=0;
                currentBatch[1]++;
            }

            if(numJobInBatch==currentJob[2]&& currentBatch[2]<numBatch)
            {

                writeStats(matrix[currentBatch[2]],traumaStats,2);
                initOutputStats(&traumaStats,SERVERSTRAUMA);
                initTime(&traumaStats,t.current);
                //partial
                writeStats(matrix[currentBatch[2]],yellowTraumaStats,5);
                writeStats(matrix[currentBatch[2]],greenTraumaStats,6);
                initOutputStats(&yellowTraumaStats, SERVERSTRAUMA);
                initTime(&yellowTraumaStats,t.current);

                initOutputStats(&greenTraumaStats, SERVERSTRAUMA);
                initTime(&greenTraumaStats,t.current);

                currentJob[2]=0;
                currentBatch[2]++;
            }
            if(numJobInBatch==currentJob[3]&& currentBatch[3]<numBatch)
            {

                writeStats(matrix[currentBatch[3]],minorStats,3);
                initOutputStats(&minorStats,SERVERSMINOR);
                initTime(&minorStats,t.current);

                //partial
                writeStats(matrix[currentBatch[3]],yellowMinorStats,7);
                writeStats(matrix[currentBatch[3]],greenMinorStats,8);
                writeStats(matrix[currentBatch[3]],whiteMinorStats,9);
                initOutputStats(&yellowMinorStats, SERVERSMINOR);
                initTime(&yellowMinorStats,t.current);

                initOutputStats(&greenMinorStats, SERVERSMINOR);
                initTime(&greenMinorStats,t.current);

                initOutputStats(&whiteMinorStats, SERVERSMINOR);
                initTime(&whiteMinorStats,t.current);
                currentJob[3]=0;
                currentBatch[3]++;
            }
            if(numJobInBatch==currentJob[4] && currentBatch[4]<numBatch)
            {

                //total
                writeStats(matrix[currentBatch[4]],medicalStats,4);
                initOutputStats(&medicalStats,SERVERSMEDICAL);
                initTime(&medicalStats,t.current);
                //partial
                writeStats(matrix[currentBatch[4]],yellowMedicalStats,10);
                writeStats(matrix[currentBatch[4]],greenMedicalStats,11);

                initOutputStats(&yellowMedicalStats, SERVERSMEDICAL);
                initTime(&yellowMedicalStats,t.current);

                initOutputStats(&greenMedicalStats, SERVERSMEDICAL);
                initTime(&greenMedicalStats,t.current);
                currentJob[4]=0;
                currentBatch[4]++;
            }

        }
        /* process an arrival */
        if(t.current==t.arrival){
            triageNumber++;
            triageStats.index++;
            if(!finite && currentJob[0]>=numJobInBatch) triageStats.index--;
            t.arrival=getArrival();
            if(finite && t.arrival>STOP){
                t.arrival=INF;
            }
            if(triageNumber<=SERVERSTRIAGE){
                int index = FindOne(triage); //find a free server
                triage[index].service=t.current+getService(triageParams);
                triage[index].occupied=1;
                t.triageCompletion=NextEvent(triage, SERVERSTRIAGE);
            }
        }

            /* completion of the triage phase and transfer to the next phase */
        else if(t.triageCompletion>-1 && t.current==triage[t.triageCompletion].service){
            triageNumber--;
            currentJob[0]++;
            //@every server data modification replaced w function row 250
            if(triageNumber-SERVERSTRIAGE>=0)
                modifyServerData(&triage[t.triageCompletion], t.current+getService(triageParams), 1);

            else
                modifyServerData(&triage[t.triageCompletion], INF, 0);

            if(triageNumber > 0)
                t.triageCompletion=NextEvent(triage, SERVERSTRIAGE);
            else
                t.triageCompletion=-1;

            color code = assignCode();
            double p;    //p for probability stuff
            double paranc;

            /* queue assignment, based on code color */
            switch (code){
                case red:
                    redNumber++;
                    redCodeStats.index++;
                    if(!finite && currentJob[1]>=numJobInBatch) redCodeStats.index--;
                    if(redNumber<=SERVERSRED){
                        int index=FindOne(redCode);
                        modifyServerData(&redCode[index], t.current+getService(redParams), 1);
                        t.redCodeCompletion = NextEvent(redCode, SERVERSRED);
                    }

                        /*people in red code may die while waiting (if they don't get served immediately)... :( */
                    else{
                        SelectStream(7);
                        p = Uniform(0,100);
                        int x = redNumber-SERVERSRED;
                        if(p<((100*pow(x,2)+20*x)/(pow(x,2)+25*x+1))){
                            redNumber--;
                            if(finite) decessi[iteration]++;
                            else decessi[currentBatch[1]]++;
                            currentJob[1]++;
                        }
                    }
                    break;

                case yellow:
                    SelectStream(8);
                    p = Uniform(0,100);
                    if(p<26.7){
                        traumaYellowNumber++;
                        traumaStats.index++;
                        yellowTraumaStats.index++;
                        if(!finite && currentJob[2]>=numJobInBatch) traumaStats.index--;
                        if(!finite && currentJob[2]>=numJobInBatch) yellowTraumaStats.index--;
                        if(traumaYellowNumber+traumaGreenNumber<=SERVERSTRAUMA){
                            traumaInServiceYellow++;
                            int index = FindOne(trauma); //find a free server
                            modifyServerDataColor(&trauma[index], t.current+getService(traumaParams), 1, yellow);
                            t.traumaCompletion=NextEvent(trauma, SERVERSTRAUMA);
                        }

                        else{


                            SelectStream(10);
                            p = Uniform(0,1);
                            if(p<probability){
                                countOrange++;
                                SelectStream(9);
                                p = Uniform(0,100);
                                int x = traumaYellowNumber-traumaInServiceYellow;
                                if(p<(100*pow(x,2)+5*x)/(pow(x,2)+33*x+1)){
                                    traumaYellowNumber--;
                                    if(finite) decessi[iteration]++;
                                    else decessi[currentBatch[2]]++;
                                    currentJob[2]++;
                                }
                            }
                        }
                    }
                    else if(p<51.4){
                        medicalYellowNumber++;
                        medicalStats.index++;
                        yellowMedicalStats.index++;
                        if(!finite && currentJob[4]>=numJobInBatch) medicalStats.index--;
                        if(!finite && currentJob[4]>=numJobInBatch) yellowMedicalStats.index--;
                        if(medicalYellowNumber+medicalGreenNumber<=SERVERSMEDICAL){
                            medicalInServiceYellow++;
                            int index = FindOne(medical);
                            modifyServerDataColor(&medical[index], t.current+getService(medicalParams), 1, yellow);
                            t.medicalCompletion=NextEvent(medical, SERVERSMEDICAL);
                        }

                        else{
                            SelectStream(10);
                            p = Uniform(0,1);
                            if(p<probability){
                                SelectStream(9);
                                p = Uniform(0,100);
                                int x = medicalYellowNumber-medicalInServiceYellow;
                                if(p<(100*pow(x,2)+5*x)/(pow(x,2)+33*x+1)){
                                    medicalYellowNumber--;
                                    if(finite) decessi[iteration]++;
                                    else decessi[currentBatch[4]]++;
                                    currentJob[4]++;
                                }
                            }
                        }
                    }
                    else{
                        minorYellowNumber++;
                        minorStats.index++;
                        yellowMinorStats.index++;
                        if(!finite && currentJob[3]>=numJobInBatch) minorStats.index--;
                        if(!finite && currentJob[3]>=numJobInBatch) yellowMinorStats.index--;
                        if(minorYellowNumber+minorGreenNumber+minorWhiteNumber<=SERVERSMINOR){
                            minorInServiceYellow++;
                            int index = FindOne(minor); //find a free server
                            modifyServerDataColor(&minor[index], t.current+getService(minorParams), 1, yellow);
                            t.minorCompletion=NextEvent(minor, SERVERSMINOR);
                        }
                        else{
                            SelectStream(10);
                            p = Uniform(0,1);
                            if(p<probability){
                                SelectStream(9);
                                p = Uniform(0,100);
                                int x = minorYellowNumber - minorInServiceYellow;
                                if(p<(100*pow(x,2)+5*x)/(pow(x,2)+33*x+1)){
                                    minorYellowNumber--;
                                    if(finite) decessi[iteration]++;
                                    else decessi[currentBatch[3]]++;
                                    currentJob[3]++;
                                }
                            }
                        }
                    }
                    break;

                case green:
                    SelectStream(8);
                    p = Uniform(0,100);
                    if(p<26.7){
                        traumaGreenNumber++;
                        traumaStats.index++;
                        greenTraumaStats.index++;
                        if(!finite && currentJob[2]>=numJobInBatch) traumaStats.index--;
                        if(!finite && currentJob[2]>=numJobInBatch) greenTraumaStats.index--;
                        if(traumaYellowNumber+traumaGreenNumber<=SERVERSTRAUMA){
                            if(traumaYellowNumber-traumaInServiceYellow==0){
                                traumaInServiceGreen++;
                                int index = FindOne(trauma);
                                modifyServerDataColor(&trauma[index], t.current+getService(traumaParams), 1, green);
                                t.traumaCompletion=NextEvent(trauma, SERVERSTRAUMA);
                            }
                        }
                    }
                    else if(p<51.4){
                        medicalGreenNumber++;
                        medicalStats.index++;
                        greenMedicalStats.index++;
                        if(!finite && currentJob[4]>=numJobInBatch) medicalStats.index--;
                        if(!finite && currentJob[4]>=numJobInBatch) greenMedicalStats.index--;
                        if(medicalYellowNumber+medicalGreenNumber<=SERVERSMEDICAL){
                            if(medicalYellowNumber-medicalInServiceYellow==0){
                                medicalInServiceGreen++;
                                int index = FindOne(medical);
                                modifyServerDataColor(&medical[index], t.current+getService(medicalParams), 1, green);
                                t.medicalCompletion=NextEvent(medical, SERVERSMEDICAL);
                            }
                        }
                    }
                    else{
                        minorGreenNumber++;
                        minorStats.index++;
                        greenMinorStats.index++;
                        if(!finite && currentJob[3]>=numJobInBatch) minorStats.index--;
                        if(!finite && currentJob[3]>=numJobInBatch) greenMinorStats.index--;
                        if(minorYellowNumber+minorGreenNumber+minorWhiteNumber<=SERVERSMINOR){
                            if(minorYellowNumber-minorInServiceYellow==0){
                                minorInServiceGreen++;
                                int index = FindOne(minor); //find a free server
                                modifyServerDataColor(&minor[index], t.current+getService(minorParams), 1, green);
                                t.minorCompletion=NextEvent(minor, SERVERSMINOR);
                            }
                        }
                    }
                    break;

                case white:
                    minorWhiteNumber++;
                    minorStats.index++;
                    whiteMinorStats.index++;
                    if(!finite && currentJob[3]>=numJobInBatch) minorStats.index--;
                    if(!finite && currentJob[3]>=numJobInBatch) whiteMinorStats.index--;
                    if(minorYellowNumber+minorGreenNumber+minorWhiteNumber<=SERVERSMINOR){
                        if((minorYellowNumber-minorInServiceYellow==0) && (minorGreenNumber-minorInServiceGreen==0)){
                            minorInServiceWhite++;
                            int index = FindOne(minor); //find a free server
                            modifyServerDataColor(&minor[index], t.current+getService(minorParams), 1, white);
                            t.minorCompletion=NextEvent(minor, SERVERSMINOR);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

            /* trauma server completion */
        else if(t.traumaCompletion>-1 && t.current==trauma[t.traumaCompletion].service){
            currentJob[2]++;
            if(trauma[t.traumaCompletion].color==yellow){
                traumaYellowNumber--;
                traumaInServiceYellow--;
            }
            else{
                traumaGreenNumber--;
                traumaInServiceGreen--;
            }
            //yellow codes have > priority than green codes
            if(traumaYellowNumber-traumaInServiceYellow>0){
                modifyServerDataColor(&trauma[t.traumaCompletion], t.current+getService(traumaParams), 1, yellow);
                traumaInServiceYellow++;
            }
            else if(traumaGreenNumber>=SERVERSTRAUMA-traumaInServiceYellow){
                modifyServerDataColor(&trauma[t.traumaCompletion], t.current+getService(traumaParams), 1, green);
                traumaInServiceGreen++;
            }
            else{
                modifyServerDataColor(&trauma[t.traumaCompletion], INF, 0, none);
            }

            if(traumaYellowNumber+traumaGreenNumber>0)
                t.traumaCompletion = NextEvent(trauma, SERVERSTRAUMA);
            else
                t.traumaCompletion=-1;
        }

            /* medical server completion */
        else if(t.medicalCompletion>-1 && t.current==medical[t.medicalCompletion].service){
            currentJob[4]++;
            if(medical[t.medicalCompletion].color==yellow){
                medicalYellowNumber--;
                medicalInServiceYellow--;
            }
            else{
                medicalGreenNumber--;
                medicalInServiceGreen--;
            }
            if(medicalYellowNumber-medicalInServiceYellow>0){
                modifyServerDataColor(&medical[t.medicalCompletion], t.current+getService(medicalParams), 1, yellow);
                medicalInServiceYellow++;
            }
            else if(medicalGreenNumber>=SERVERSMEDICAL-medicalInServiceYellow){
                modifyServerDataColor(&medical[t.medicalCompletion], t.current+getService(medicalParams), 1, green);
                medicalInServiceGreen++;
            }
            else{
                modifyServerDataColor(&medical[t.medicalCompletion], INF, 0, none);
            }

            if(medicalYellowNumber+medicalGreenNumber>0)
                t.medicalCompletion = NextEvent(medical, SERVERSMEDICAL);
            else
                t.medicalCompletion=-1;
        }

            /* minor problems server completion */
        else if(t.minorCompletion>-1 && t.current==minor[t.minorCompletion].service){
            currentJob[3]++;
            if(minor[t.minorCompletion].color==yellow){
                minorYellowNumber--;
                minorInServiceYellow--;
            }
            else if(minor[t.minorCompletion].color==green){
                minorGreenNumber--;
                minorInServiceGreen--;
            }
            else{
                minorWhiteNumber--;
                minorInServiceWhite--;
            }
            if(minorYellowNumber-minorInServiceYellow>0){
                modifyServerDataColor(&minor[t.minorCompletion], t.current+getService(minorParams), 1, yellow);
                minorInServiceYellow++;
            }
            else if(minorGreenNumber-minorInServiceGreen>0){
                modifyServerDataColor(&minor[t.minorCompletion], t.current+getService(minorParams), 1, green);
                minorInServiceGreen++;
            }
            else if(minorWhiteNumber>=SERVERSMINOR-minorInServiceYellow-minorInServiceGreen){
                modifyServerDataColor(&minor[t.minorCompletion], t.current+getService(minorParams), 1, white);
                minorInServiceWhite++;
            }
            else{
                modifyServerDataColor(&minor[t.minorCompletion], INF, 0, none);
            }

            if(minorYellowNumber+minorGreenNumber+minorWhiteNumber>0)
                t.minorCompletion = NextEvent(minor, SERVERSMINOR);
            else
                t.minorCompletion=-1;
        }

            /* red code queue completion */
        else if(t.redCodeCompletion>-1 && t.current==redCode[t.redCodeCompletion].service){
            currentJob[1]++;
            redNumber--;
            if(redNumber-SERVERSRED>=0){
                modifyServerData(&redCode[t.redCodeCompletion], t.current+getService(redParams), 1);
            }
            else{
                modifyServerData(&redCode[t.redCodeCompletion], INF, 0);
            }
            if(redNumber > 0)
                t.redCodeCompletion=NextEvent(redCode, SERVERSRED);
            else
                t.redCodeCompletion=-1;
        }

    }
    //debug
    writeStats(matrix[iteration],triageStats,0);
    writeStats(matrix[iteration],redCodeStats,1);
    decessi[iteration]=decessi[iteration]/(redCodeStats.index+yellowTraumaStats.index+yellowMinorStats.index+yellowMedicalStats.index);
    writeStats(matrix[iteration],traumaStats,2);
    writeStats(matrix[iteration],minorStats,3);
    writeStats(matrix[iteration],medicalStats,4);
    writeStats(matrix[iteration],yellowTraumaStats,5);
    writeStats(matrix[iteration],greenTraumaStats,6);
    writeStats(matrix[iteration],yellowMinorStats,7);
    writeStats(matrix[iteration],greenMinorStats,8);
    writeStats(matrix[iteration],whiteMinorStats,9);
    writeStats(matrix[iteration],yellowMedicalStats,10);
    writeStats(matrix[iteration],greenMedicalStats,11);

    return 0;
}
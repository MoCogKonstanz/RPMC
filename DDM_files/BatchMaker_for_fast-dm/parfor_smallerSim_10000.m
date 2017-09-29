
%import
filename = 'C:\Users\Patrick\Desktop\FRONTIERS\_DIFFUSION\_ACCURACY-coding-with-errors\EX1\with-error-cutoff\cutoff-1800\MODEL_check\ex1params.txt';
delimiter = '\t';
formatSpec = '%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%[^\n\r]';
fileID = fopen(filename,'r');
dataArray = textscan(fileID, formatSpec, 'Delimiter', delimiter, 'TextType', 'string',  'ReturnOnError', false);
fclose(fileID);
ex1params1 = table(dataArray{1:end-1}, 'VariableNames', {'VarName1','VarName2','VarName3','VarName4','VarName5','VarName6','VarName7','VarName8','VarName9','VarName10','VarName11','VarName12','VarName13','VarName14','VarName15','VarName16','VarName17','VarName18','VarName19','VarName20','VarName21','VarName22','VarName23','VarName24','VarName25','VarName26','VarName27','VarName28','VarName29','VarName30','VarName31','VarName32','VarName33'});

%% Clear temporary variables
clearvars filename delimiter formatSpec fileID dataArray ans;

%% actual code
%exp: 2x2x2 design
%2 levels each for condition, task, and grip

%condition dependent drift params:
%a, st0, t0, and v

%d varies per subject, z fixed to 0.5
%others: default

%parameter table: 24 rows (subjects) x 33 (params; 8a + 8st0 + 8t0 + 8v + d)

%

ex1setcov = cov(ex1params); %gives 33x33 array

ex1simulated =[];

parfor ii = 1:10000
    
    ex1temp = mvnrnd(ex1params, ex1setcov);
    
    ex1simulated = vertcat(ex1simulated,ex1temp);
    
end

%results in 1008000x33 array

%%
%make condition tables
simulatedtable = array2table(ex1simulated,...
    'VariableNames',{'a_Blocked_Plan_Overhand',...
    'a_Blocked_Plan_Underhand', 'a_Blocked_Rule_Overhand',...
    'a_Blocked_Rule_Underhand', 'a_Mixed_Plan_Overhand',...
    'a_Mixed_Plan_Underhand', 'a_Mixed_Rule_Overhand',...
    'a_Mixed_Rule_Underhand',... 
    'd',... 
    'st0_Blocked_Plan_Overhand',...
    'st0_Blocked_Plan_Underhand', 'st0_Blocked_Rule_Overhand',...
    'st0_Blocked_Rule_Underhand', 'st0_Mixed_Plan_Overhand',...
    'st0_Mixed_Plan_Underhand', 'st0_Mixed_Rule_Overhand',...
    'st0_Mixed_Rule_Underhand', 't0_Blocked_Plan_Overhand',...
    't0_Blocked_Plan_Underhand', 't0_Blocked_Rule_Overhand',...
    't0_Blocked_Rule_Underhand', 't0_Mixed_Plan_Overhand',...
    't0_Mixed_Plan_Underhand', 't0_Mixed_Rule_Overhand',...
    't0_Mixed_Rule_Underhand', 'v_Blocked_Plan_Overhand',...
    'v_Blocked_Plan_Underhand', 'v_Blocked_Rule_Overhand',...
    'v_Blocked_Rule_Underhand', 'v_Mixed_Plan_Overhand',...
    'v_Mixed_Plan_Underhand', 'v_Mixed_Rule_Overhand',...
    'v_Mixed_Rule_Underhand'});

%%
%split table by 8 conditions

ex1_BPO = simulatedtable(:,[1 9 10 18 26]); %numbers are columns corresponding to conditions
ex1_BPU = simulatedtable(:,[2 9 11 19 27]);
ex1_BRO = simulatedtable(:,[3 9 12 20 28]);
ex1_BRU = simulatedtable(:,[4 9 13 21 29]);

ex1_MPO = simulatedtable(:,[5 9 14 22 30]);
ex1_MPU = simulatedtable(:,[6 9 15 23 31]);
ex1_MRO = simulatedtable(:,[7 9 16 24 32]);
ex1_MRU = simulatedtable(:,[8 9 17 25 33]);

%% %%
%build ridiculous windows batch file from tables for contruct samples
%1st: each subject and condition 10000 rows 5 columns, 
%make 6th column with additional batch info for construct samples:

batchnfo = {'-z 0.5 -Z 0 -V 0 -n 32 -N 1 -p 5 -r -o'};
batchnfo = repmat(batchnfo,10000,1);
%"after -o: "XXX.txt" XXX should be VP+COND+RowNumber
%driftparams for batch
drifta = {'-a'};
drifta = repmat(drifta,10000,1);
driftd = {'-d'};
driftd = repmat(driftd,10000,1);
driftT = {'-T'};
driftT = repmat(driftT,10000,1);
driftt = {'-t'};
driftt = repmat(driftt,10000,1);
driftv = {'-v'};
driftv = repmat(driftv,10000,1);

%program vector
constr = {'construct-samples.exe'};
constr = repmat(constr,10000,1);

%make subject tables by combining every 24th row begining with 1st row 
%for subj 1 etc...
numvec = (1:10000)';
numvec = string(numvec);

%loop helpers:
subs = {'VP03', 'VP04', 'VP05', 'VP06', 'VP10', 'VP11', 'VP12', 'VP13',...
    'VP14', 'VP15', 'VP16', 'VP20', 'VP21', 'VP22', 'VP23', 'VP24',...
    'VP25', 'VP26', 'VP27', 'VP29', 'VP30', 'VP31', 'VP32', 'VP33'};
conds = {'BPO' 'BPU' 'BRO' 'BRU'...
    'MPO' 'MPU' 'MRO' 'MRU'};
condvar = {'ex1_BPO' 'ex1_BPU' 'ex1_BRO' 'ex1_BRU'...
    'ex1_MPO' 'ex1_MPU' 'ex1_MRO' 'ex1_MRU'};

clear jj
clear kk

outputextension = {'.txt'};
outputextension = repmat(outputextension,10000,1);

%ALL

for kk = 1:numel(conds)
    if kk == 1
        hungry = ex1_BPO;
        
    elseif kk == 2  
        hungry = ex1_BPU;
    elseif kk == 3  
        hungry = ex1_BRO;
    elseif kk == 4  
        hungry = ex1_BRU;       
    elseif kk == 5  
        hungry = ex1_MPO;
     elseif kk == 6  
        hungry = ex1_MPU;
    elseif kk == 7  
        hungry = ex1_MRO;
    elseif kk == 8  
        hungry = ex1_MRU;       
    end
    newdir = char(condvar(kk));
    mkdir(newdir)
    cd(newdir)
    
    for jj = 1:numel(subs)
        vpdir = char(subs(jj));
        mkdir(vpdir);
        cd(vpdir)
        tagged = hungry(jj:24:end,:);
        
        
        %name stuff for batch subject files
        fname = strcat(subs(jj),'_',conds(kk),'.bat','');
        output = fname;
        fname = char(fname);
        
        %outputfile column for 'construct-samples.exe' -o
        output1 = repmat(output,10000,1);
        output2 = strcat(output1,numvec,outputextension);
        output3 = cellstr(output2);

        %batch stuff
        tagged = horzcat(constr,...
            drifta,tagged(:,1),...
            driftd,tagged(:,2),...
            driftT,tagged(:,3),...
            driftt,tagged(:,4),...
            driftv,tagged(:,5),...
            batchnfo,output3...
            );
        

        
  
        
        writetable(tagged,fname,'WriteVariableNames',false,'Delimiter','tab','FileType','text')
        %up one level for next sub
        cd ..\
    end
    %back to top directory
    %cd 'C:\Users\Patrick\Desktop\FRONTIERS\_DIFFUSION\_ACCURACY-coding-with-errors\EX1\with-error-cutoff\cutoff-1800\MODEL_check\'
    cd ..\;

 end


  
%% Gather, merge, and tag simulated data


% create depends vectors (32 trials per condition-triplet)

%ex1 blocked mixed, ex2 real pantomime
blocked = {'blocked'};
blocked  = repmat(blocked,160000,1);
mixed = {'mixed'};
mixed = repmat(mixed,160000,1);
real = {'real'};
real = repmat(real,160000,1);
pantomime = {'pant'};
pantomime = repmat(pantomime,160000,1);

%task
rule = {'rule'};
rule = repmat(rule,160000,1);
plan = {'plan'};
plan = repmat(plan,160000,1);

%grip

overhand = {'overhand'};
overhand = repmat(overhand,160000,1);
underhand = {'underhand'};
underhand = repmat(underhand,160000,1);



%%
%gather 5000 simsets per sub and condition

%from this directory:


for mm = 1:numel(subs)
cd 'F:\_Drift_ex1\10000_samp_sims-gathered'
cd 'ex1_BPO'
cd(char(subs(mm)));
sim = importdata('merged.txt');
sim = sim(1:160000,:);
sim = num2cell(sim);
idx = subs(mm);
idx = repelem(idx, 160000);
tagvec = horzcat(idx',blocked,plan,overhand);
BPOmerge = horzcat(tagvec,sim);

cd ../
cd ../

cd 'ex1_BPU'
cd(char(subs(mm)));
sim = importdata('merged.txt');
sim = sim(1:160000,:);
sim = num2cell(sim);
idx = subs(mm);
idx = repelem(idx, 160000);
tagvec = horzcat(idx',blocked,plan,underhand);
BPUmerge = horzcat(tagvec,sim);

cd ../
cd ../

cd 'ex1_BRO'
cd(char(subs(mm)));
sim = importdata('merged.txt');
sim = sim(1:160000,:);
sim = num2cell(sim);
idx = subs(mm);
idx = repelem(idx, 160000);
tagvec = horzcat(idx',blocked,rule,overhand);
BROmerge = horzcat(tagvec,sim);

cd ../
cd ../

cd 'ex1_BRU'
cd(char(subs(mm)));
sim = importdata('merged.txt');
sim = sim(1:160000,:);
sim = num2cell(sim);
idx = subs(mm);
idx = repelem(idx, 160000);
tagvec = horzcat(idx',blocked,rule,underhand);
BRUmerge = horzcat(tagvec,sim);
 
cd ../
cd ../

cd 'ex1_MPO'
cd(char(subs(mm)));
sim = importdata('merged.txt');
sim = sim(1:160000,:);
sim = num2cell(sim);
idx = subs(mm);
idx = repelem(idx, 160000);
tagvec = horzcat(idx',mixed,plan,overhand);
MPOmerge = horzcat(tagvec,sim);

cd ../
cd ../

cd 'ex1_MPU'
cd(char(subs(mm)));
sim = importdata('merged.txt');
sim = sim(1:160000,:);
sim = num2cell(sim);
idx = subs(mm);
idx = repelem(idx, 160000);
tagvec = horzcat(idx',mixed,plan,underhand);
MPUmerge = horzcat(tagvec,sim);

cd ../
cd ../

cd 'ex1_MRO'
cd(char(subs(mm)));
sim = importdata('merged.txt');
sim = sim(1:160000,:);
sim = num2cell(sim);
idx = subs(mm);
idx = repelem(idx, 160000);
tagvec = horzcat(idx',mixed,rule,overhand);
MROmerge = horzcat(tagvec,sim);

cd ../
cd ../

cd 'ex1_MRU'
cd(char(subs(mm)));
sim = importdata('merged.txt');
sim = sim(1:160000,:);
sim = num2cell(sim);
idx = subs(mm);
idx = repelem(idx, 160000);
tagvec = horzcat(idx',mixed,rule,underhand);
MRUmerge = horzcat(tagvec,sim);
%numfiles = length(textFiles);
%mydata = cell(1, numfiles);
%namehelper = strcat(subs(mm),'_BPO.bat%d.txt'); 
%namehelper = char(namehelper);

%for nn = 1:numfiles
   %myfilename = sprintf(namehelper, nn);
  %myfilename = sprintf(namehelper, nn);
  %mydata{nn} = importdata(myfilename);
  %mydata{nn} = importdata(myfilename);
  %eval(['load ' textFiles(nn).name ' -ascii']);
  %BPOsimloop = importdata(textFiles(nn).name);
  %BPOsim = vertcat(BPOsim,BPOsimloop);
%end
cd ../
cd ../


%mkdir 'simsets'
cd 'simsets'

mkdir(char(subs(mm)));
cd(char(subs(mm)));

    for gg = 1:5000
        
        
        homie = gg*32;
        
        inni = homie-31;
       outBPO = BPOmerge(inni:homie,:);
       outBPU = BPUmerge(inni:homie,:);
       outBRO = BROmerge(inni:homie,:);
       outBRU = BRUmerge(inni:homie,:);
       
       outMPO = MPOmerge(inni:homie,:);
       outMPU = MPUmerge(inni:homie,:);
       outMRO = MROmerge(inni:homie,:);
       outMRU = MRUmerge(inni:homie,:);
       
       firstline = {'# *', 'CONDITION', 'TASK', 'GRIP', 'RESPONSE', 'TIME'};
       
       oneset = vertcat(firstline, outBPO, outBPU, outBRO, outBRU,...
           outMPO, outMPU, outMRO, outMRU);
       
       oneset = array2table(oneset);
       setnum = strcat(subs(mm),'_',string(gg),'.txt');
       
       
       writetable(oneset,setnum,'WriteVariableNames',false,'Delimiter','tab','FileType','text')
    
       
    
    end

end




cd 'F:\_rev2_sim\_FULL_redo_all-100k\ex2'
%import

filename = 'F:\_rev2_sim\empirical\ex2params.txt';
delimiter = '\t';
formatSpec = '%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f%[^\n\r]';
fileID = fopen(filename,'r');
dataArray = textscan(fileID, formatSpec, 'Delimiter', delimiter, 'TextType', 'string',  'ReturnOnError', false);
fclose(fileID);
ex2params = [dataArray{1:end-1}];


%% Clear temporary variables
clearvars filename delimiter formatSpec fileID dataArray ans;

%% actual code
%exp: 2x2 design
%2 levels each for condition and task

%condition dependent drift params:
%a, st0, t0, and v

%d varies per subject, z fixed to 0.5
%others: default

%parameter table: 24(16 in expA=exp2) rows (subjects) x 17 (params; 4a + 4st0 + 4t0 + 4v + d)



ex2setcov = cov(ex2params); %gives 17x17 array
%%
ex2simulated =[];

parfor ii = 1:100000
    
    ex2temp = mvnrnd(ex2params, ex2setcov);
    
    ex2simulated = vertcat(ex2simulated,ex2temp);
    
end

%results in 240000x17 array

%%
%make condition tables
simulatedtable = array2table(ex2simulated,...
    'VariableNames',{'a_Pantomime_Plan',...
    'a_Pantomime_Rule',...
    'a_Real_Plan',...
    'a_Real_Rule',... 
    'd',... 
    'st0_Pantomime_Plan',...
    'st0_Pantomime_Rule',...
    'st0_Real_Plan',...
    'st0_Real_Rule',...
    't0_Pantomime_Plan',...
    't0_Pantomime_Rule',...
    't0_Real_Plan',...
    't0_Real_Rule',...
    'v_Pantomime_Plan',...
    'v_Pantomime_Rule',...
    'v_Real_Plan',...
    'v_Real_Rule'});

%%
%split table by 4 conditions

ex2_PP = simulatedtable(:,[1 5 6 10 14]); %numbers are columns corresponding to conditions
ex2_PR = simulatedtable(:,[2 5 7 11 15]);
ex2_RP = simulatedtable(:,[3 5 8 12 16]);
ex2_RR = simulatedtable(:,[4 5 9 13 17]);



%% %%
%build ridiculous windows batch file from tables for contruct samples
%1st: each subject and condition 100000 rows 5 columns, 
%make 6th column with additional batch info for construct samples:

batchnfo = {'-z 0.5 -Z 0 -V 0 -n 64 -N 1 -p 4 -r -o'};
batchnfo = repmat(batchnfo,100000,1);
%"after -o: "XXX.txt" XXX should be VP+COND+RowNumber
%driftparams for batch
drifta = {'-a'};
drifta = repmat(drifta,100000,1);
driftd = {'-d'};
driftd = repmat(driftd,100000,1);
driftT = {'-T'};
driftT = repmat(driftT,100000,1);
driftt = {'-t'};
driftt = repmat(driftt,100000,1);
driftv = {'-v'};
driftv = repmat(driftv,100000,1);

%program vector
constr = {'construct-samples.exe'};
constr = repmat(constr,100000,1);

%make subject tables by combining every 24th row begining with 1st row 
%for subj 1 etc...
numvec = (1:100000)';
numvec = string(numvec);

%loop helpers:
subs = {'VP01', 'VP02', 'VP03', 'VP04', 'VP05', 'VP06', 'VP07', 'VP08',...
    'VP09', 'VP10', 'VP11', 'VP12', 'VP13', 'VP14', 'VP15', 'VP16'};
conds = {'PP' 'PR'...
    'RP' 'RR'};
condvar = {'ex2_PP' 'ex2_PR'...
    'ex2_RP' 'ex2_RR'};

clear jj
clear kk

outputextension = {'.txt'};
outputextension = repmat(outputextension,100000,1);

%ALL

for kk = 1:numel(conds)
    if kk == 1
        hungry = ex2_PP;
        
    elseif kk == 2  
        hungry = ex2_PR;
    elseif kk == 3  
        hungry = ex2_RP;
    elseif kk == 4  
        hungry = ex2_RR;            
    end
    newdir = char(condvar(kk));
    mkdir(newdir)
    cd(newdir)
    
    for jj = 1:numel(subs)
        vpdir = char(subs(jj));
        mkdir(vpdir);
        cd(vpdir)
        tagged = hungry(jj:numel(subs):end,:);
        
        
        %name stuff for batch subject files
        fname = strcat(subs(jj),'_',conds(kk),'.bat','');
        output = fname;
        fname = char(fname);
        
        %outputfile column for 'construct-samples.exe' -o
        output1 = repmat(output,100000,1);
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
    %cd 'C:\Users\Patrick\Desktop\FRONTIERS\_DIFFUSION\_ACCURACY-coding-with-errors\ex2\with-error-cutoff\cutoff-1800\MODEL_check\'
    cd ..\;

 end


%% Gather, merge, and tag simulated data


% create depends vectors (32 trials per condition-triplet)

%ex1 blocked mixed, ex2 real pantomime
blocked = {'blocked'};
blocked  = repmat(blocked,320000,1);
mixed = {'mixed'};
mixed = repmat(mixed,320000,1);
real = {'real'};
real = repmat(real,320000,1);
pantomime = {'pant'};
pantomime = repmat(pantomime,320000,1);

%task
rule = {'rule'};
rule = repmat(rule,320000,1);
plan = {'plan'};
plan = repmat(plan,320000,1);

%grip

%overhand = {'overhand'};
%overhand = repmat(overhand,160000,1);
%underhand = {'underhand'};
%underhand = repmat(underhand,160000,1);

  
%%
%gather 5000 simsets per sub and condition

%from this directory:
cd 'F:\_rev2_sim\_FULL_redo_all-100k\ex2\ex2-make-samples'
mkdir 'simsets'
for mm = 1:numel(subs)
cd 'F:\_rev2_sim\_FULL_redo_all-100k\ex2\ex2-make-samples'
cd 'ex2_PP'
cd(char(subs(mm)));
sim = importdata('merged.txt');
sim = sim(1:320000,:);
sim = num2cell(sim);
idx = subs(mm);
idx = repelem(idx, 320000);
tagvec = horzcat(idx',pantomime,plan);
PPmerge = horzcat(tagvec,sim);

cd ../
cd ../

cd 'ex2_PR'
cd(char(subs(mm)));
sim = importdata('merged.txt');
sim = sim(1:320000,:);
sim = num2cell(sim);
idx = subs(mm);
idx = repelem(idx, 320000);
tagvec = horzcat(idx',pantomime,rule);
PRmerge = horzcat(tagvec,sim);

cd ../
cd ../


cd 'ex2_RP'
cd(char(subs(mm)));
sim = importdata('merged.txt');
sim = sim(1:320000,:);
sim = num2cell(sim);
idx = subs(mm);
idx = repelem(idx, 320000);
tagvec = horzcat(idx',real,plan);
RPmerge = horzcat(tagvec,sim);

cd ../
cd ../

cd 'ex2_RR'
cd(char(subs(mm)));
sim = importdata('merged.txt');
sim = sim(1:320000,:);
sim = num2cell(sim);
idx = subs(mm);
idx = repelem(idx, 320000);
tagvec = horzcat(idx',real,rule);
RRmerge = horzcat(tagvec,sim);


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



cd 'simsets'

mkdir(char(subs(mm)));
cd(char(subs(mm)));

    for gg = 1:5000
        
        
        homie = gg*64;
        
        inni = homie-63;
       outPP = PPmerge(inni:homie,:);
       outPR = PRmerge(inni:homie,:);
       outRP = RPmerge(inni:homie,:);
       outRR = RRmerge(inni:homie,:);
       
       
       firstline = {'# *', 'CONDITION', 'TASK', 'RESPONSE', 'TIME'};
       
       oneset = vertcat(firstline, outPP, outPR, outRP, outRR);
       
       oneset = array2table(oneset);
       setnum = strcat(subs(mm),'_',string(gg),'.txt');
       
       
       writetable(oneset,setnum,'WriteVariableNames',false,'Delimiter','tab','FileType','text')
    
       
    
    end

end
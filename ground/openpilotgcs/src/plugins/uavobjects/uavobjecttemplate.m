function [] = OPLogConvert()
    %% Define indices and arrays of structures to hold data
$(ALLOCATIONCODE)
   
    %% Open file
    %fid = fopen('log.opl');
    [FileName,PathName,FilterIndex] = uigetfile('*.opl');
    logfile = strcat(PathName,FileName);
    fid = fopen(logfile);
    
    while (1)
        %% Read logging header        
        timestamp = fread(fid, 1, 'uint32');
        if (feof(fid)); break; end
        datasize = fread(fid, 1, 'int64');
          
        
        %% Read message header        
        % get sync field (0x3C, 1 byte)
        sync = fread(fid, 1, 'uint8');
        if sync ~= hex2dec('3C')
            disp ('Wrong sync byte');
            return
        end        
        % get msg type (quint8 1 byte ) should be 0x20, ignore the rest?
        msgType = fread(fid, 1, 'uint8');
        if msgType ~= hex2dec('20')
            disp ('Wrong msgType');
            return
        end        
        % get msg size (quint16 2 bytes) excludes crc, include msg header and data payload
        msgSize = fread(fid, 1, 'uint16');        
        % get obj id (quint32 4 bytes)
        objID = fread(fid, 1, 'uint32');        
        
        
        %% Read object
        switch objID
$(SWITCHCODE)                
            otherwise
                disp('Unknown object ID');
                msgBytesLeft = datasize - 1 - 1 - 2 - 4;
                fread(fid, msgBytesLeft, 'uint8');
        end
        
    end
    
    %% Clean Up and Save mat file
    fclose(fid);
    
    matfile = strrep(logfile,'opl','mat');
    save(matfile $(SAVEOBJECTSCODE));
    
end


%% Object reading functions
$(FUNCTIONSCODE)



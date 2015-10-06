function [img, dim, dty, dun, dvs, dnc, cid] = MIV_Load(miv_file)
    fid = fopen(miv_file, 'r');
    sig = fread(fid, [1,4], '*char');
    if ~strcmp(sig, 'MIVF')
       fclose(fid);
       return
    end
    dim = fread(fid, [1,3], '*uint32');
    dty = fread(fid, [1,1], '*uint32');
    dun = fread(fid, [1,1], '*uint32');
    dvs = fread(fid, [1,3], '*double');
    dnc = fread(fid, [1,1], '*uint32');
    cid = fread(fid, [1,8], '*uint8');
    count = [1,dim(1) * dim(2) * dim(3)];
    switch dty
        case 0
            img = fread(fid, count, '*uint8');
        case 1
            img = fread(fid, count, '*int16');
        case 2
            img = fread(fid, count, '*uint16');
        case 3
            img = fread(fid, count, '*int32');
        case 4
            img = fread(fid, count, '*float');
        case 5
            img = fread(fid, count, '*uint8');
        case 6
            img = fread(fid, [3,count(2)], '*uint8');
        case 11
            img = fread(fid, count, '*uint32');
        case 12
            img = fread(fid, [4,count(2)], '*uint8');
        case 13
            img = fread(fid, count, '*uint32');
        case 16
            img = fread(fid, count, '*double');
    end
    fclose(fid);

    img = reshape(img, dim);
end
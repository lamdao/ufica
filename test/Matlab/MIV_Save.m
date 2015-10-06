function MIV_Save(fn, img)
    dim = size(img);
    if length(dim) == 2,
        dim = [dim(1), dim(2), 1];
    end
    count = [1,dim(1) * dim(2) * dim(3)];
    fp = fopen(fn, 'w');
    fwrite(fp, 'MIVF', '*char');
    fwrite(fp, dim, '*uint32');
    fwrite(fp, 2, '*uint32');
    fwrite(fp, 0, '*uint32');
    fwrite(fp, [1.0, 1.0, 1.0], '*double');
    fwrite(fp, 0, '*uint32');
    fwrite(fp, [0,0,0,0,0,0,0,0], '*uint8');
    fwrite(fp, reshape(img, count), '*uint16');
    fclose(fp);
end
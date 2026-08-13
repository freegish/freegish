package gishleveltool;

import java.awt.image.BufferedImage;
import java.awt.image.RenderedImage;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.EOFException;
import java.math.BigInteger;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Arrays;
import java.util.ArrayList;
import javax.imageio.ImageIO;

public class Gishlevel {

    static int BITYPE = BufferedImage.TYPE_4BYTE_ABGR;
    byte[] version = new byte[4];
    byte[] level_background = new byte[32];
    byte[] level_tileset = new byte[4];
    byte[] level_gametype = new byte[4];
    byte[] level_time = new byte[4];
    byte[][] level_area = new byte[256][4];
    byte[][] level_backgrid = new byte[256 * 256][1];
    byte[][] level_grid = new byte[256 * 256][1];
    byte[][] level_foregrid = new byte[256 * 256][1];
    byte[][] level_startposition = new byte[3][4];
    byte[][] level_ambient = new byte[12][4];
    byte[] level_numofobjects = new byte[4];
    byte[] objects_data;// = new byte[(4 + 4 + 4 + (4 * 3) + 4 + (4 * 2) + 4 + 4 + 4 + (4 * 3) + 4 + 4) * (int) readCInt(level_numofobjects)];
    byte[] level_numofropes = new byte[4];
    byte[] ropes_data;// = new byte[(4 + 4 + 4 + 4 + 4 + 4) * readCInt(level_numofropes)];
    byte[] block_friction = new byte[4];
    byte[] block_breakpoint = new byte[4];
    byte[] block_middamage = new byte[4];
    byte[] block_foredamage = new byte[4];
    byte[] block_density = new byte[4];
    byte[] block_drag = new byte[4];
    byte[] block_animation = new byte[4];
    byte[] block_animationspeed = new byte[4];
    //ArrayList<byte[]> textures_and_blocks = new ArrayList<byte[]>();
    ArrayList<Tile> tiles = new ArrayList<Tile>();

    public void loadlevel(String path) throws FileNotFoundException {
        File f = new File(path);
        FileInputStream fs = new FileInputStream(f);
        try {
            processFile(fs);
            fs.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static Byte[] btoB(byte[] b1) {
        Byte[] b2 = new Byte[b1.length];
        for (int i = 0; i < b1.length; i++) {
            b2[i] = b1[i];
        }
        return b2;
    }

    public static byte[] Btob(Byte[] b1) {
        byte[] b2 = new byte[b1.length];
        for (int i = 0; i < b1.length; i++) {
            b2[i] = b1[i];
        }
        return b2;
    }

    public static byte[] cat(byte[] in1, byte[] in2) {
        byte[] ret = new byte[in1.length + in2.length];
        int count = 0;
        for (int i = 0; i < in1.length; i++) {
            ret[count] = in1[i];
            count++;
        }
        for (int i = 0; i < in2.length; i++) {
            ret[count] = in2[i];
            count++;
        }
        return ret;
    }

    public void processFile(FileInputStream fs) throws IOException, NoSuchAlgorithmException {

        ArrayList<RenderedImage> images = new ArrayList<RenderedImage>();

        fs.read(version);
        int offset, read;

        for (offset = 0; offset < level_background.length; offset += read) {
            read = fs.read(level_background, offset, level_background.length - offset);
            if (read < 0) throw new EOFException("Failed to read level background");
        }

        // Need to use trim() here otherwise length() always returns 32.
        String fns = new String(level_background).trim();

        if (fns.endsWith(".tga")) {
            Arrays.fill(level_background, fns.length() - 4, level_background.length, (byte) 0);
        }

        fs.read(level_tileset);
        fs.read(level_gametype);
        fs.read(level_time);
        for (int i = 0; i < level_area.length; i++) {
            int j = fs.read(level_area[i]);
            //System.out.println("Read bytes for " + i + ": " + j);
        }

        for (int i = 0; i < level_backgrid.length; i++) {
            int j = fs.read(level_backgrid[i]);
            //System.out.println("Read bytes for " + i + ": " + j);
        }
        for (int i = 0; i < level_grid.length; i++) {
            int j = fs.read(level_grid[i]);
            //System.out.println("Read bytes for " + i + ": " + j);
        }
        for (int i = 0; i < level_foregrid.length; i++) {
            int j = fs.read(level_foregrid[i]);
            //System.out.println("Read bytes for " + i + ": " + j);
        }

        for (int i = 0; i < level_startposition.length; i++) {
            int j = fs.read(level_startposition[i]);
            //System.out.println("Read bytes for " + i + ": " + j);
        }
        for (int i = 0; i < level_ambient.length; i++) {
            int j = fs.read(level_ambient[i]);
            //System.out.println("Read bytes for " + i + ": " + j);
        }
        fs.read(level_numofobjects);

        objects_data = new byte[(4 + 4 + 4 + (4 * 3) + 4 + (4 * 2) + 4 + 4 + 4 + (4 * 3) + 4 + 4) * (int) readCInt(level_numofobjects)];
        fs.read(objects_data);

        fs.read(level_numofropes);

        ropes_data = new byte[(4 + 4 + 4 + 4 + 4 + 4) * readCInt(level_numofropes)];
        fs.read(ropes_data);

        for (int i = 1; i < 251; i++) {
            Tile tile = new Tile();
            //TEXTURE
            byte[] sizex = new byte[4];
            fs.read(sizex);
            //addtextureblock(sizex);
            tile.put("sizex", sizex);
            if (readCInt(sizex) < 0) {
                /*
                fread2(filename, 1, filenameLength, fp);
                 */
                int filenamelength = Math.abs(readCInt(sizex));
                //System.out.println(" Filenamelength for tex" + i + ":" + filenamelength);
                if (filenamelength > 256) {
                    System.err.println("ULP! filenamelength:" + filenamelength);
                    filenamelength = 256;
                }
                byte[] filename = new byte[filenamelength];
                fs.read(filename);
                String fname = new String(filename);
                //System.out.println("Tex" + i + " -- REFERENCE -- " + fname);
                tile.put("filename", filename);
                //addtextureblock(filename);




            } else if (readCInt(sizex) == 0) {
            } else {
                byte[] sizey = new byte[4];
                byte[] magfilter = new byte[4];
                byte[] minfilter = new byte[4];
                fs.read(sizey);
                fs.read(magfilter);
                fs.read(minfilter);
                tile.put("sizey", sizey);
                tile.put("magfilter", magfilter);
                tile.put("minfilter", minfilter);
                int sx = readCInt(sizex);
                int sy = readCInt(sizey);


                byte[][] textureblob = new byte[sx * sy][4];
                int temp_tbsize = sx * sy * 4;
                byte[] textureblob_single = new byte[temp_tbsize];
                for (int ti = 0; ti < textureblob.length; ti++) {
                    fs.read(textureblob[ti]);
                }


                for (int ia = 0; ia < textureblob.length; ia++) {
                    for (int ib = 0; ib < 4; ib++) {
                        textureblob_single[(ia * 4) + ib] = textureblob[ia][ib];
                    }
                }

                tile.put("textureblob", textureblob_single);
            }
            byte[] block_numoflines = new byte[4];
            fs.read(block_numoflines);
            tile.put("block_numoflines", block_numoflines);

            byte[] block_lines = new byte[8 * 4 * readCInt(block_numoflines)];
            fs.read(block_lines);
            tile.put("block_lines", block_lines);

            block_friction = new byte[4];
            block_breakpoint = new byte[4];
            block_middamage = new byte[4];
            block_foredamage = new byte[4];
            block_density = new byte[4];
            block_drag = new byte[4];
            block_animation = new byte[4];
            block_animationspeed = new byte[4];

            fs.read(block_friction);
            tile.put("block_friction", block_friction);
            fs.read(block_breakpoint);
            tile.put("block_breakpoint", block_breakpoint);
            fs.read(block_middamage);
            tile.put("block_middamage", block_middamage);
            fs.read(block_foredamage);
            tile.put("block_foredamage", block_foredamage);
            fs.read(block_density);
            tile.put("block_density", block_density);
            fs.read(block_drag);
            tile.put("block_drag", block_drag);
            fs.read(block_animation);
            tile.put("block_animation", block_animation);
            fs.read(block_animationspeed);
            tile.put("block_animationspeed", block_animationspeed);

            tiles.add(tile);
        }

        System.out.println("Version: " + readCInt(version));
    }

    public void savelevel(String path) {
        try {
            FileOutputStream fos = new FileOutputStream(new File(path));

            fos.write(version);
            fos.write(level_background);
            fos.write(level_tileset);
            fos.write(level_gametype);
            fos.write(level_time);
            write_dbarray(level_area, fos);
            write_dbarray(level_backgrid, fos);
            write_dbarray(level_grid, fos);
            write_dbarray(level_foregrid, fos);
            write_dbarray(level_startposition, fos);
            write_dbarray(level_ambient, fos);
            fos.write(level_numofobjects);
            fos.write(objects_data);
            fos.write(level_numofropes);
            fos.write(ropes_data);
            for (Tile t : tiles) {
                byte[] writeB = t.getTileData();
                //System.err.println("Writing tile with tileData size: " + t.getTileData().length);
                fos.write(writeB);
            }

            fos.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void write_dbarray(byte[][] in, FileOutputStream writer) throws IOException {
        for (int i = 0; i < in.length; i++) {
            writer.write(in[i]);
        }
    }

    public static int readCInt(byte[] input) throws IOException {
        ByteArrayInputStream bis = new ByteArrayInputStream(input);
        DataInputStream in = new DataInputStream(bis);
        long retint = 0;
        for (int i = 0; i <
                4; i++) {
            int j = in.readUnsignedByte();
            retint +=
                    j * (Math.pow(2, i * 8));
        }

        if (retint > 2147483647) {
            retint -= 2147483647;
            retint -=
                    2147483647;
            retint -=
                    2;
        }

        return (int) retint;
    }

    public static byte[] inttoBarray(int number) {
        byte[] intByte = new byte[4];
        for (int i = 0; i < 4; i++) {
            intByte[i] = (byte) (number >> (8 * i));
        }
        return intByte;
    }

    public static float readCFloat(byte[] input) throws IOException {
        ByteArrayInputStream bis = new ByteArrayInputStream(input);
        DataInputStream in = new DataInputStream(bis);
        return in.readFloat();
    }

    private static BufferedImage getImagefromByteArray(byte[] barray, int sizex, int sizey) throws IOException {
        BufferedImage bufferedImage = new BufferedImage(sizex, sizey, BITYPE);

        int i = 0;
        for (int y = 0; y < sizey; y++) {
            for (int x = 0; x < sizex; x++) {
                byte[] b = new byte[4];
                b[2] = barray[i];
                b[1] = barray[i + 1];
                b[0] = barray[i + 2];
                b[3] = barray[i + 3];
                bufferedImage.setRGB(x, y, readCInt(b));
                i += 4;
            }
        }
        return bufferedImage;
    }

    public void extract_and_convert(ArrayList<MD5db> md5dbs) throws IOException {
        if (readCInt(version) == 10) {
            version = inttoBarray(11);

            File dir = new File("texture/converted");
            if (!dir.exists()) {
                System.out.println("Making directory texture/converted");
                dir.mkdir();
            }

            try {

                for (Tile t : tiles) {
                    if (t.get("sizex") != null && t.get("sizey") != null && t.get("textureblob") != null) {


                        int sizex = readCInt(t.get("sizex"));
                        int sizey = readCInt(t.get("sizey"));
                        if (sizex > 0) {
                            if (t.get("filename") == null) {
                                byte[] texdata = t.get("textureblob");
                                if (texdata != null) {
                                    BufferedImage image = getImagefromByteArray(texdata, sizex, sizey);
                                    String md5 = getMD5fromImage(image);
                                    String filenameTo = "";
                                    String tempString;
                                    boolean success = false;
                                    for (MD5db m : md5dbs) {
                                        if (!success) {
                                            tempString = m.getFile(md5);
                                            if (tempString != null) {
                                                filenameTo = tempString;
                                                success = true;
                                            }
                                        }
                                    }
                                    if (!success) {
                                        filenameTo = "texture/converted/" + md5 + ".png";
                                        saveImage(image, filenameTo);
                                    }

                                    //Image saved, or reference caught.
                                    
                                    if (filenameTo.startsWith("texture/")) {
                                        filenameTo = filenameTo.substring("texture/".length());
                                    }
                                    //System.err.println("Filenameto: " + filenameTo);
                                    t.put("sizex", inttoBarray(-filenameTo.length()));
                                    t.put("sizey", null);
                                    t.put("textureblob", null);
                                    t.put("magfilter", null);
                                    t.put("minfilter", null);
                                    t.put("filename", filenameTo.getBytes());
                                }
                            }
                        }
                    }
                }

            } catch (Exception e) {
                e.printStackTrace();
            }
        } else {
            System.err.println("WARNING: Cannot convert: level is version " + readCInt(version) + " instead of 10");
        }
    }

    public static String getMD5fromImage(BufferedImage image) throws IOException, NoSuchAlgorithmException {
        MessageDigest md = MessageDigest.getInstance("MD5");

        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        ImageIO.write(image, "png", baos);
        md.update(baos.toByteArray());
        BigInteger bi = new BigInteger(1, md.digest());
        return bi.toString(16);
    }

    public static void saveImage(BufferedImage image, String path) throws IOException {
        ImageIO.write(image, "png", new File(path));
    }
}

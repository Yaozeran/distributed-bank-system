package common;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;

public class Marshaller {

    // === Core Type Marshalling ===

    public static void packInt(ByteBuffer buf, int value) {
        buf.order(ByteOrder.LITTLE_ENDIAN);
        buf.putInt(value);
    }

    public static int unpackInt(ByteBuffer buf) {
        buf.order(ByteOrder.LITTLE_ENDIAN);
        return buf.getInt();
    }

    public static void packDouble(ByteBuffer buf, double value) {
        buf.order(ByteOrder.LITTLE_ENDIAN);
        buf.putDouble(value);
    }

    public static double unpackDouble(ByteBuffer buf) {
        buf.order(ByteOrder.LITTLE_ENDIAN);
        return buf.getDouble();
    }

    public static void packLong(ByteBuffer buf, long value) {
        buf.order(ByteOrder.LITTLE_ENDIAN);
        buf.putLong(value);
    }

    public static long unpackLong(ByteBuffer buf) {
        buf.order(ByteOrder.LITTLE_ENDIAN);
        return buf.getLong();
    }

    public static void packString(ByteBuffer buf, String text) {
        buf.order(ByteOrder.LITTLE_ENDIAN);
        if (text == null) text = "";
        byte[] bytes = text.getBytes(StandardCharsets.UTF_8);
        buf.putLong((long)bytes.length);
        buf.put(bytes);
    }

    public static String unpackString(ByteBuffer buf) {
        buf.order(ByteOrder.LITTLE_ENDIAN);
        long len = buf.getLong();
        if (len <= 0) {
            return "";
        }
        if (len > Integer.MAX_VALUE) {
            throw new IllegalArgumentException("String length too large: " + len);
        }
        byte[] bytes = new byte[(int) len];
        buf.get(bytes);
        return new String(bytes, StandardCharsets.UTF_8);
    }

    public static void packFloat(ByteBuffer buf, float value) {
        buf.order(ByteOrder.LITTLE_ENDIAN);
        buf.putFloat(value);
    }

    public static float unpackFloat(ByteBuffer buf) {
        buf.order(ByteOrder.LITTLE_ENDIAN);
        return buf.getFloat();
    }


    // === Legacy Support for string-only packing (for tests) ===
    // Keeps the old method to avoid breaking existing tests immediately, 
    // or we can update tests.
    public static byte[] packString(String text) {
        if (text == null) {
            text = "";
        }
        byte[] bytes = text.getBytes(StandardCharsets.UTF_8);
        ByteBuffer buf = ByteBuffer.allocate(4 + bytes.length);
        buf.order(ByteOrder.LITTLE_ENDIAN);
        buf.putInt(bytes.length);
        buf.put(bytes);
        return buf.array();
    }
}

import java.io.FileInputStream
import java.nio.ByteBuffer
import java.nio.channels.FileChannel

def scanFile(filename: String) {
  val channel = new FileInputStream(filename).getChannel()

  val buffer = ByteBuffer.allocate(32 * 1024)

  var count = 0

  do {
    count = channel.read(buffer)
    buffer.clear()
  } while (count != -1)

  channel.close()
}

argv match {
  case Array(filename) => {
    (1 to 100).foreach { _ => scanFile(filename) }

    val start = System.currentTimeMillis
    scanFile(filename)
    println("Read in " + (System.currentTimeMillis - start) + "ms")
  }
  case _ => println("Usage: test <filename>")
}

sys.exit() 

require "zip"

def wrap_and_flush(zipfile, dest, source)
  ignore_start = Regexp.new("(.*)// {{(.*)")
  ignore_end = "// }}"

  ignoring = false

  reader = File.new(source, "r")
  writer = zipfile.get_output_stream(dest)
  reader.readlines().each do |line|
    match = line.match(ignore_start)
    if not match.nil?
      writer.write(match[1] + match[2] + "\n")
      ignoring = true
    end

    if not ignoring
      writer.write(line)
    end

    if line.strip == ignore_end
      ignoring = false
    end
  end
  reader.close()
  writer.close()

  return
end

Jekyll::Hooks.register :site, :post_write do |site|
  site.data["archives"].each do |name, inputs|
    # Create archive directory
    destdir = File.join(site.dest, "archives")
    if not Dir.exists?(destdir)
      Dir.mkdir(destdir)
    end

    # Retrieve input files and exclusion patterns
    files = []
    excludes = []
    inputs.each do |input|
      if input.is_a?(String)
        files.append(input)
      elsif input.is_a?(Hash)
        if not input["exclude"].nil?
          input["exclude"].each do |exc|
            excludes.append(Regexp.new(exc))
          end
        end
      end
    end

    tempfile = File.join(site.dest, "temp.txt")

    # Create ZIP archives
    Zip.continue_on_exists_proc = true
    Zip::File.open(File.join(destdir, name + ".zip"), Zip::File::CREATE) do |zipfile|
      files.each do |file|
        if File.file?(file)
          zipfile.add(File.basename(file), File.join(site.config["source"], file))
        elsif File.directory?(file)
          Dir.glob(file + "/**/*.*").each do |subfile|
            is_include = true
            excludes.each do |exc|
              if not subfile.match(exc).nil?
                is_include = false
                break
              end
            end

            if is_include
              dest = subfile.sub(file + "/", "")
              wrap_and_flush(zipfile, dest, subfile)
            end
          end
        end
      end
    end
  end
end

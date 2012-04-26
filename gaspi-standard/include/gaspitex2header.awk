#!/bin/awk -f

# Helper script to save work (not a full blown application)
# run as: ./gaspitex2header.awk <path_to_tex_file>
# OR
#         ./gaspitex2header.awk <path_to_tex_file> > GASPI.h
# and then modify what's missing

BEGIN {
#FS = "FDef";
    functions = 0;
    total_params="";
    total_f="";
    retn="";
    docs = 1;
    types = 1;
    code = 0;

    
}

/\\TDefDefn/ {
     if( $0 !~ /^%/ )
      {
	  type=$0;
	  gsub(/\\TDefDefn{/,"typedef ", type); #
	  gsub(/{\\gaspitype/, " gaspi_", type);
	  gsub(/\\_/,"_", type);
	  gsub(/{/,"", type);
	  gsub(/}/,"", type);
	  type=sprintf("%s_t;\n", type);
      }
}

/\\TDefDesc/ {
    if( $0 !~ /^%/ )
    {
	type_comm_start="/*!\n\\typedef \n\\brief ";
	type_comm_end="\\details \n*/"
	type_desc=$0;
	gsub(/\\TDefDesc{/,"", type_desc);
	gsub(/\\GASPI\{\}/, "GASPI", type_desc);
	gsub(/}/, "", type_desc);
	type_desc =sprintf("%s%s\n%s\n", type_comm_start, type_desc, type_comm_end);
    } 
    
}

/\\end{TDef}/ {
    if(types == 1)
    {
	if(docs == 1)
	{
	    print type_desc;
	}
	
	print type;
    }
    
}


/\\begin{verbatim}/ {
    
    c=0;
    functions++;
    if(functions == 1)
    {
	if(code == 0)
	{
	    print "#ifndef _GASPI_H_"
	    print "#define _GASPI_H_ 1"
	    print "/********************************************************/";
	    print "/*                                                      */";
	    print "/*                                                      */";
	    print "/*                                                      */";
	    print "/*                 GASPI Interface                      */";
	    print "/*                                                      */";
	    print "/*                                                      */";
	    print "/*                                                      */";
	    print "/********************************************************/";
	    print "\n";
	}
	else
	{
	    print "#include <GASPI.h>\n";
	    
	}
    }
    
#    print c
}

/\\end{verbatim}/ {

    for (i = 1; i < c; i++)
	total_f=sprintf("%s\n%s here i am", total_f, x[i]);
}

/^\\parameterlistbegin/ {
    p=0;
}

/^\\parameterlistitem/{
    params[p]=$0;
    p++;
}

/^\\parameterlistend/{

    for(i = 0; i < p; i++)
    {
	sub(/\\parameterlistitem/, "\\param ", params[i]);
	gsub(/{in}/, "[in]", params[i]);
	gsub(/{out}/, "[out]", params[i]);
	gsub(/{/," ", params[i]);
	gsub(/}/,"  ", params[i]);

	total_params=sprintf("%s\n%s", total_params, params[i]);
    }

    header="//! \n/*!\n\n";
    closer="*/";
    if(docs == 0) 
      {
	header="";
	total_params="";
	retn="";
	closer="";
      }

    endline=";";

    if(code == 1)
    {
	endline = sprintf("\n{\n\treturn GASPI_SUCCESS;\n}\n");
	
    }
    

    printf "%s%s\n%s%s\n%s%s\n\n", header,total_params,retn,closer, total_f, endline;
    total_params="";
    total_f="";
    retn="";
}

 /^\\FStdRetDescNOTimeout/{
   retn="\return ";
   retn+="GASPI_SUCCESS: operation has successfully returned";
   retn+="GASPI_ERROR: operation has terminated with an error";
}
 /^\\FStdRetDesc/{
   retn="\\return gaspi_return_t\n";
   retn=retn "\\retval GASPI_SUCCESS: operation has successfully returned\n";
   retn=retn "\\retval GASPI_TIMEOUT: operation has run into timeout\n";
   retn=retn "\\retval GASPI_ERROR: operation has terminated with an error";
}


{
    if( $0 !~ /^%/ )
    {
	x[c]=$0;
	c++;
    }
    
}

END{
    print "#endif"
    printf "Num of functions %d\n", functions > "/dev/stderr";

}
	       

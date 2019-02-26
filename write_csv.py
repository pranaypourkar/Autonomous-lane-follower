import xlsxwriter

wb = xlsxwriter.Workbook('sample-test.xlsx')
ws = wb.add_worksheet('my sheet')

# write the header in row 0, which is Excel row 1
ws.write_row(0, 0, ['angle'])
    
ws.write_row(i+1, 0, [angle])

wb.close()